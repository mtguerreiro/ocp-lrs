# Open the existing project
open_project ./pynq_lrssoc/pynq_lrssoc.xpr

# Regenerate wrapper (optional if you’re using block design)
# make_wrapper -files [get_files *.bd] -top -force
# add_files ./<wrapper>.v
# update_compile_order -fileset sources_1

# Run synthesis
launch_runs synth_1 -jobs 4
wait_on_run synth_1

# Run implementation
launch_runs impl_1 -to_step write_bitstream -jobs 4
wait_on_run impl_1

# Export hardware platform with bitstream
write_hw_platform -fixed -include_bit -force ./pynq_lrssoc.xsa

puts "XSA exported to ./pynq_lrssoc.xsa"
exit
