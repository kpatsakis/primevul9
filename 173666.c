static void svg_ps(char *p)
{
	char *q;

	for (;;) {
		q = strstr(p, "<defs>");
		if (!q)
			break;
		p = strstr(q, "</defs>");
		if (!p) {
			error(1, NULL, "No </defs> in %%beginsvg");
			break;
		}
		parse_defs(q + 6, p);
	}
}