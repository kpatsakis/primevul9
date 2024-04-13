xps_parse_point(char *s_in, float *x, float *y)
{
	char *s_out = s_in;
	float xy[2];

	s_out = xps_parse_float_array(s_out, 2, &xy[0]);
	*x = xy[0];
	*y = xy[1];
	return s_out;
}