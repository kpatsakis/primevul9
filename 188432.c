xps_parse_real_num(char *s, float *number)
{
	char buf[64];
	char *p = buf;
	while (is_real_num_char(*s))
		*p++ = *s++;
	*p = 0;
	if (buf[0])
		*number = fz_atof(buf);
	return s;
}