xps_parse_poly_quadratic_bezier_segment(fz_context *doc, fz_path *path, fz_xml *root, int stroking, int *skipped_stroke)
{
	char *points_att = fz_xml_att(root, "Points");
	char *is_stroked_att = fz_xml_att(root, "IsStroked");
	float x[2], y[2];
	int is_stroked;
	fz_point pt;
	char *s;
	int n;

	if (!points_att)
	{
		fz_warn(doc, "PolyQuadraticBezierSegment element has no points");
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
		if (n == 2)
		{
			if (stroking && !is_stroked)
			{
				fz_moveto(doc, path, x[1], y[1]);
			}
			else
			{
				pt = fz_currentpoint(doc, path);
				fz_curveto(doc, path,
						(pt.x + 2 * x[0]) / 3, (pt.y + 2 * y[0]) / 3,
						(x[1] + 2 * x[0]) / 3, (y[1] + 2 * y[0]) / 3,
						x[1], y[1]);
			}
			n = 0;
		}
	}
}