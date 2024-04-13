seamless_select_timeout(struct timeval *tv)
{
	struct timeval ourtimeout = { 0, SEAMLESSRDP_POSITION_TIMER };

	if (g_seamless_rdp)
	{
		if (timercmp(&ourtimeout, tv, <))
		{
			tv->tv_sec = ourtimeout.tv_sec;
			tv->tv_usec = ourtimeout.tv_usec;
		}
	}
}