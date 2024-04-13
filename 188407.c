xps_parse_rectangle(xps_document *doc, char *text, fz_rect *rect)
{
	float args[4];
	char *s = text;
	int i;

	args[0] = 0; args[1] = 0;
	args[2] = 1; args[3] = 1;

	for (i = 0; i < 4 && *s; i++)
	{
		args[i] = fz_atof(s);
		while (*s && *s != ',')
			s++;
		if (*s == ',')
			s++;
	}

	rect->x0 = args[0];
	rect->y0 = args[1];
	rect->x1 = args[0] + args[2];
	rect->y1 = args[1] + args[3];
}