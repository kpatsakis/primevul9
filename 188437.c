xps_parse_poly_line_segment(fz_context *doc, fz_path *path, fz_xml *root, int stroking, int *skipped_stroke)
{
	char *points_att = fz_xml_att(root, "Points");
	char *is_stroked_att = fz_xml_att(root, "IsStroked");
	int is_stroked;
	float x, y;
	char *s;

	if (!points_att)
	{
		fz_warn(doc, "PolyLineSegment element has no points");
		return;
	}

	is_stroked = 1;
	if (is_stroked_att && !strcmp(is_stroked_att, "false"))
			is_stroked = 0;
	if (!is_stroked)
		*skipped_stroke = 1;

	s = points_att;
	while (*s != 0)
	{
		while (*s == ' ') s++;
		s = xps_parse_point(s, &x, &y);
		if (stroking && !is_stroked)
			fz_moveto(doc, path, x, y);
		else
			fz_lineto(doc, path, x, y);
	}
}