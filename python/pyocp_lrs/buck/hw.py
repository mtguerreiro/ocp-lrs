"""
Module ``buck_hw``
===================


"""
import pyocp
import struct

class Commands:
    """
    """
    def __init__(self):
        self.set_pwm_enable = 0
        self.get_pwm_enable = 1
        
        self.set_pwm_freq = 2
        self.get_pwm_freq = 3
        
        self.set_pwm_duty = 4
        self.get_pwm_duty = 5
        
        self.set_pwm_dead_time = 6
        self.get_pwm_dead_time = 7

        self.set_input_relay = 8
        self.get_input_relay = 9

        self.set_output_relay = 10
        self.get_output_relay = 11

        self.set_meas_gains = 12
        self.get_meas_gains = 13

        self.clear_status = 14
        self.get_status = 15


class MeasGains:

    def __init__(self):

        self.keys = (
            'io_gain', 'io_ofs', 'il_gain', 'il_ofs',
            'v_dc_out_gain', 'v_dc_out_ofs', 'v_out_gain', 'v_out_ofs',
            'v_dc_in_gain', 'v_dc_in_ofs', 'v_in_gain', 'v_in_ofs',
            'il_avg_gain', 'il_avg_ofs'
        )

    def encode(self, gains):

        keys = self.keys
        
        _gains = [gains[key] for key in keys]
        gains_bin = struct.pack(f'<{len(keys)}f', *_gains)

        return gains_bin

    
    def decode(self, data_bin):

        keys = self.keys
        
        data = struct.unpack(f'<{len(keys)}f', data_bin)
        gains = dict(zip(keys, data))

        return gains


class Hw:
    """

    Parameters
    ----------

    Raises
    ------

    Attributes
    ----------
        
    """
    def __init__(self, ocp_if, cs_id):

        self._cmd = Commands()
        self._ocp_if = ocp_if
        self._cs_id = cs_id
        self._meas_gains = MeasGains()


    def set_pwm_enable(self, enable):

        return self._set_pwm_enable(enable)


    def get_pwm_enable(self):

        return self._get_pwm_enable()
    

    def set_pwm_frequency(self, freq):
        """Sets PWM frequency, in Hz.
        """
        return self._set_pwm_freq(int(freq))


    def get_pwm_frequency(self):
        """Gets PWM frequency, in Hz.
        """
        status, freq = self._get_pwm_freq()
        if status != 0:
            return (-1, status)
        
        return (status, int(freq))
    

    def set_pwm_duty(self, duty):
        """Sets PWM duty (0 < duty < 1).
        """
        return self._set_pwm_duty(float(duty))


    def get_pwm_duty(self):
        """Gets PWM duty, in percentage.
        """
        status, duty = self._get_pwm_duty()
        if status != 0:
            return(-1, status)
        
        return (0, float(duty))
    

    def set_pwm_dead_time(self, dead_time):
        """Sets PWM dead time, in ns.
        """
        return self._set_pwm_dead_time(float(int(dead_time) * 1e-9))


    def get_pwm_dead_time(self):
        """Gets PWM dead time, in ns.
        """
        status, dt = self._get_pwm_dead_time()
        if status != 0:
            return (-1, status)

        return (0, round(float(dt / 1e-9)))


    def set_input_relay(self, state):
        """Sets the input relay.
        """
        return self._set_input_relay(int(state))


    def get_load_switch(self):
        """Gets the input relay.
        """
        status, state = self._get_input_relay()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))


    def set_output_relay(self, state):
        """Sets the output relay.
        """
        return self._set_output_relay(int(state))


    def get_output_relay(self):
        """Gets the output relay.
        """
        status, state = self._get_output_relay()
        if status != 0:
            return (-1, status)
        
        return (status, int(state))


    def get_meas_gains(self):
        """Gets conversion gains.
        """
        status, gains_bin = self._get_meas_gains()
        if status != 0:
            return (-1, status)

        gains = self._meas_gains.decode(gains_bin)
        
        return (status, gains)


    def set_meas_gains(self, gains):
        """Sets conversion gains.
        """
        gains_bin = self._meas_gains.encode(gains)
        status = self._set_meas_gains(gains_bin)

        if status[0] != 0:
            return (-1, status[1])
        
        return (0,)


    def clear_status(self):
        """Clears the hardware status.
        """
        return self._clear_status()

    
    def get_status(self):
        """Gets hardware status
        """
        status, hw_status = self._get_status()
        if status != 0:
            return (-1, status)
        
        return (status, hw_status)

    
    def _set_pwm_enable(self, reset):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_enable

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(reset, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM reset. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_enable(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_pwm_enable

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, reset = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM reset. Error code {:}\r\n'.format(status))
            return (-1, status)

        reset = pyocp.conversions.u8_to_u32(reset, msb=False)
        
        return (0, reset)


    def _set_pwm_freq(self, freq):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.set_pwm_freq

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(freq, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM frequency. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_freq(self):
        """

        Parameters
        ----------

        Raises
        ------

        """    
        cmd = self._cmd.get_pwm_freq

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, freq = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM frequency. Error code {:}\r\n'.format(status))
            return (-1, status)

        freq = pyocp.conversions.u8_to_u32(freq, msb=False)
        
        return (0, freq)


    def _set_pwm_duty(self, duty):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_pwm_duty
        
        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', duty)) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM duty. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_duty(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_pwm_duty
        
        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, duty_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting PWM duty. Error code {:}\r\n'.format(status))
            return (-1, status)

        duty = struct.unpack('<f', duty_b)[0]
        
        return (0, duty)


    def _set_pwm_dead_time(self, dead_time):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_pwm_dead_time

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( list(struct.pack('<f', dead_time)) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM dead time. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_pwm_dead_time(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_pwm_dead_time

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, dt_b = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting PWM dead time. Error code {:}\r\n'.format(status))
            return (-1, status)

        dt = struct.unpack('<f', dt_b)[0]
        
        return (0, dt)


    def _set_input_relay(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_input_relay

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the load switch. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_input_relay(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_input_relay

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the load switch. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)


    def _set_output_relay(self, state):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_output_relay

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( pyocp.conversions.u32_to_u8(state, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the output switch. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)


    def _get_output_relay(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_output_relay

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, state = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting the output switch. Error code {:}\r\n'.format(status))
            return (-1, status)

        state = pyocp.conversions.u8_to_u32(state, msb=False)
        
        return (0, state)


    def _get_meas_gains(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_meas_gains

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, gains = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting meas gains. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0, gains)


    def _set_meas_gains(self, gains_bin):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.set_meas_gains

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        tx_data.extend( gains_bin )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error setting the meas gains. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)

    
    def _clear_status(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.clear_status

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, _ = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error clearing the hardware status. Error code {:}\r\n'.format(status))
            return (-1, status)
        
        return (0,)

    
    def _get_status(self):
        """

        Parameters
        ----------

        Raises
        ------

        """
        cmd = self._cmd.get_status

        tx_data = []
        tx_data.extend( pyocp.conversions.u32_to_u8(cmd, msb=False) )
        
        status, hw_status = self._ocp_if.cs_hardware_if(self._cs_id, tx_data)

        if status < 0:
            print('Error getting hardware status. Error code {:}\r\n'.format(status))
            return (-1, status)

        hw_status = pyocp.conversions.u8_to_u32(hw_status, msb=False)
        
        return (0, hw_status)
