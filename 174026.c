doSleep(int iSeconds, int iuSeconds)
{
	struct timeval tvSelectTimeout;
	tvSelectTimeout.tv_sec = iSeconds;
	tvSelectTimeout.tv_usec = iuSeconds; /* micro seconds */
	select(0, NULL, NULL, NULL, &tvSelectTimeout);
}