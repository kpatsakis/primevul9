xps_parse_glyph_metrics(char *s, float *advance, float *uofs, float *vofs)
{
	if (*s == ',')
		s = xps_parse_real_num(s + 1, advance);
	if (*s == ',')
		s = xps_parse_real_num(s + 1, uofs);
	if (*s == ',')
		s = xps_parse_real_num(s + 1, vofs);
	return s;
}