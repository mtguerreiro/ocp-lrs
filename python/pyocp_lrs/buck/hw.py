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

"""
class MeasGains:

    def __init__(self):
        pass

    def decode(self, data):
        fmt = '<' + 'f' * 20
        data = struct.unpack(fmt, data)

        gains = {
            'i_i_gain':      data[0],  'i_i_ofs':      data[1],
            'i_1_gain':      data[2],  'i_1_ofs':      data[3],
            'v_in_gain':     data[4],  'v_in_ofs':     data[5],
            'v_dc_gain':     data[6],  'v_dc_ofs':     data[7],
            'v_1_gain':      data[8],  'v_1_ofs':      data[9],
            'i_o_gain':      data[10], 'i_o_ofs':      data[11],
            'i_2_gain':      data[12], 'i_2_ofs':      data[13],
            'v_out_gain':    data[14], 'v_out_ofs':    data[15],
            'v_dc_out_gain': data[16], 'v_dc_out_ofs': data[17],
            'v_2_gain':      data[18], 'v_2_ofs':      data[19],
            }

        return gains

    def encode(self, gains):
        data = [gains['i_i_gain'],      gains['i_i_ofs'],
                gains['i_1_gain'],      gains['i_1_ofs'],
                gains['v_in_gain'],     gains['v_in_ofs'],
                gains['v_dc_gain'],     gains['v_dc_ofs'],
                gains['v_1_gain'],      gains['v_1_ofs'],
                gains['i_o_gain'],      gains['i_o_ofs'],
                gains['i_2_gain'],      gains['i_2_ofs'],
                gains['v_out_gain'],    gains['v_out_ofs'],
                gains['v_dc_out_gain'], gains['v_dc_out_ofs'],
                gains['v_2_gain'],      gains['v_2_ofs']
                ]
        fmt = '<' + 'f' * 20
        data = struct.pack(fmt, *data)

        return data
"""
        
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
        status, gains = self._get_meas_gains()
        if status != 0:
            return (-1, status)
        
        return (status, gains)


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
