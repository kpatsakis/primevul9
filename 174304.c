rdpsnd_reset_state(void)
{
	if (device_open)
		current_driver->wave_out_close();
	device_open = False;
	rdpsnd_queue_clear();
	rdpsnd_negotiated = False;

	if (rec_device_open)
		current_driver->wave_in_close();
	rec_device_open = False;
	rdpsnd_clear_record();
	rdpsnd_rec_negotiated = False;
}