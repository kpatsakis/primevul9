rdpsnd_check_fds(fd_set * rfds, fd_set * wfds)
{
	rdpsnd_queue_complete_pending();

	if (device_open || rec_device_open)
		current_driver->check_fds(rfds, wfds);
}