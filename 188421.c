xps_parse_cluster_mapping(char *s, int *code_count, int *glyph_count)
{
	if (*s == '(')
		s = xps_parse_digits(s + 1, code_count);
	if (*s == ':')
		s = xps_parse_digits(s + 1, glyph_count);
	if (*s == ')')
		s ++;
	return s;
}