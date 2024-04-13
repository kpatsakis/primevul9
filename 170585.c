static const char *dt2str(unsigned long ticks)
{
	static char buf[32];

	sprintf(buf, "%ld.%02ld", ticks / HZ, ((ticks % HZ) * 100) / HZ);

	return buf;
}