static void parse_defs(char *p, char *q)
{
	char *id, *r;
	int idsz;

	for (;;) {
		id = strstr(p, "id=\"");
		if (!id || id > q)
			return;
		r = strchr(id + 4, '"');
		if (!r)
			return;
		idsz = r + 1 - id;

		// if SVG output, mark the id as defined
		if (svg || epsf > 1) {
			svg_def_id(id, idsz);
			p = r;
			continue;
		}

		// convert SVG to PS
		p = id;
		while (*p != '<')
			p--;
		if (strncmp(p, "<path ", 6) == 0) {
			parse_path(p + 6, q, id, idsz);
			p = strstr(p, "/>");
			if (!p)
				break;
			p += 2;
			continue;
		}
		break;
	}
}