rdpsnd_add_fds(int *n, fd_set * rfds, fd_set * wfds, struct timeval *tv)
{
	long next_pending;

	if (device_open || rec_device_open)
		current_driver->add_fds(n, rfds, wfds, tv);

	next_pending = rdpsnd_queue_next_completion();
	if (next_pending >= 0)
	{
		long cur_timeout;

		cur_timeout = tv->tv_sec * 1000000 + tv->tv_usec;
		if (cur_timeout > next_pending)
		{
			tv->tv_sec = next_pending / 1000000;
			tv->tv_usec = next_pending % 1000000;
		}
	}
}