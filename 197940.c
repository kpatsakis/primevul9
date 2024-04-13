static int gettime(void)
{
	static struct timeval first;
	static int once = 1;
	struct timeval now;
	if (once)
	{
		gettimeofday(&first, NULL);
		once = 0;
	}
	gettimeofday(&now, NULL);
	return (now.tv_sec - first.tv_sec) * 1000 + (now.tv_usec - first.tv_usec) / 1000;
}