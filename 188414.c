xps_parse_path_figure(fz_context *doc, fz_path *path, fz_xml *root, int stroking)
{
	fz_xml *node;

	char *is_closed_att;
	char *start_point_att;
	char *is_filled_att;

	int is_closed = 0;
	int is_filled = 1;
	float start_x = 0;
	float start_y = 0;

	int skipped_stroke = 0;

	is_closed_att = fz_xml_att(root, "IsClosed");
	start_point_att = fz_xml_att(root, "StartPoint");
	is_filled_att = fz_xml_att(root, "IsFilled");

	if (is_closed_att)
		is_closed = !strcmp(is_closed_att, "true");
	if (is_filled_att)
		is_filled = !strcmp(is_filled_att, "true");
	if (start_point_att)
		xps_parse_point(start_point_att, &start_x, &start_y);

	if (!stroking && !is_filled) /* not filled, when filling */
		return;

	fz_moveto(doc, path, start_x, start_y);

	for (node = fz_xml_down(root); node; node = fz_xml_next(node))
	{
		if (!strcmp(fz_xml_tag(node), "ArcSegment"))
			xps_parse_arc_segment(doc, path, node, stroking, &skipped_stroke);
		if (!strcmp(fz_xml_tag(node), "PolyBezierSegment"))
			xps_parse_poly_bezier_segment(doc, path, node, stroking, &skipped_stroke);
		if (!strcmp(fz_xml_tag(node), "PolyLineSegment"))
			xps_parse_poly_line_segment(doc, path, node, stroking, &skipped_stroke);
		if (!strcmp(fz_xml_tag(node), "PolyQuadraticBezierSegment"))
			xps_parse_poly_quadratic_bezier_segment(doc, path, node, stroking, &skipped_stroke);
	}

	if (is_closed)
	{
		if (stroking && skipped_stroke)
			fz_lineto(doc, path, start_x, start_y); /* we've skipped using fz_moveto... */
		else
			fz_closepath(doc, path); /* no skipped segments, safe to closepath properly */
	}
}