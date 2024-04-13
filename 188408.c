xps_parse_poly_bezier_segment(fz_context *doc, fz_path *path, fz_xml *root, int stroking, int *skipped_stroke)
{
	char *points_att = fz_xml_att(root, "Points");
	char *is_stroked_att = fz_xml_att(root, "IsStroked");
	float x[3], y[3];
	int is_stroked;
	char *s;
	int n;

	if (!points_att)
	{
		fz_warn(doc, "PolyBezierSegment element has no points");
		return;
	}

	is_stroked = 1;
	if (is_stroked_att && !strcmp(is_stroked_att, "false"))
			is_stroked = 0;
	if (!is_stroked)
		*skipped_stroke = 1;

	s = points_att;
	n = 0;
	while (*s != 0)
	{
		while (*s == ' ') s++;
		s = xps_parse_point(s, &x[n], &y[n]);
		n ++;
		if (n == 3)
		{
			if (stroking && !is_stroked)
				fz_moveto(doc, path, x[2], y[2]);
			else
				fz_curveto(doc, path, x[0], y[0], x[1], y[1], x[2], y[2]);
			n = 0;
		}
	}
}