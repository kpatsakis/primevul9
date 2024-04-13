cdf_ctime(const time_t *sec, char *buf)
{
	char *ptr = ctime_r(sec, buf);
	if (ptr != NULL)
		return buf;
	(void)snprintf(buf, 26, "*Bad* 0x%16.16llx\n", (long long)*sec);
	return buf;
}