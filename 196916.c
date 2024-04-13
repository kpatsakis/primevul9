my_gettimeofday(struct timeval *tvp, void *tzp)
{
	struct timeb timebuffer;

	ftime(&timebuffer);
	if (tvp) {
		tvp->tv_sec = timebuffer.time;
		tvp->tv_usec = timebuffer.millitm * 1000L;
	}
	return (0);
}