static const char *overlay_name_from_nr(int nr)
{
	static char buf[256];

	snprintf(buf, sizeof(buf) - 1,
		"overlay_%d", nr);
	buf[sizeof(buf) - 1] = '\0';

	return buf;
}