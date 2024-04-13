xps_parse_render_transform(xps_document *doc, char *transform, fz_matrix *matrix)
{
	float args[6];
	char *s = transform;
	int i;

	args[0] = 1; args[1] = 0;
	args[2] = 0; args[3] = 1;
	args[4] = 0; args[5] = 0;

	for (i = 0; i < 6 && *s; i++)
	{
		args[i] = fz_atof(s);
		while (*s && *s != ',')
			s++;
		if (*s == ',')
			s++;
	}

	matrix->a = args[0]; matrix->b = args[1];
	matrix->c = args[2]; matrix->d = args[3];
	matrix->e = args[4]; matrix->f = args[5];
}