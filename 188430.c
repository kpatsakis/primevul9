xps_parse_line_cap(char *attr)
{
	if (attr)
	{
		if (!strcmp(attr, "Flat")) return 0;
		if (!strcmp(attr, "Round")) return 1;
		if (!strcmp(attr, "Square")) return 2;
		if (!strcmp(attr, "Triangle")) return 3;
	}
	return 0;
}