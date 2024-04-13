xps_parse_float_array(char *s, int num, float *x)
{
	int k = 0;

	if (s == NULL || *s == 0)
		return NULL;

	while (*s)
	{
		while (*s == 0x0d || *s == '\t' || *s == ' ' || *s == 0x0a)
			s++;
		x[k] = (float)strtod(s, &s);
		while (*s == 0x0d || *s == '\t' || *s == ' ' || *s == 0x0a)
			s++;
		if (*s == ',')
			s++;
		if (++k == num)
			break;
	}
	return s;
}