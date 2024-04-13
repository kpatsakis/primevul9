xps_parse_arc_segment(fz_context *doc, fz_path *path, fz_xml *root, int stroking, int *skipped_stroke)
{
	/* ArcSegment pretty much follows the SVG algorithm for converting an
	 * arc in endpoint representation to an arc in centerpoint
	 * representation. Once in centerpoint it can be given to the
	 * graphics library in the form of a postscript arc. */

	float rotation_angle;
	int is_large_arc, is_clockwise;
	float point_x, point_y;
	float size_x, size_y;
	int is_stroked;

	char *point_att = fz_xml_att(root, "Point");
	char *size_att = fz_xml_att(root, "Size");
	char *rotation_angle_att = fz_xml_att(root, "RotationAngle");
	char *is_large_arc_att = fz_xml_att(root, "IsLargeArc");
	char *sweep_direction_att = fz_xml_att(root, "SweepDirection");
	char *is_stroked_att = fz_xml_att(root, "IsStroked");

	if (!point_att || !size_att || !rotation_angle_att || !is_large_arc_att || !sweep_direction_att)
	{
		fz_warn(doc, "ArcSegment element is missing attributes");
		return;
	}

	is_stroked = 1;
	if (is_stroked_att && !strcmp(is_stroked_att, "false"))
			is_stroked = 0;
	if (!is_stroked)
		*skipped_stroke = 1;

	point_x = point_y = 0;
	size_x = size_y = 0;

	xps_parse_point(point_att, &point_x, &point_y);
	xps_parse_point(size_att, &size_x, &size_y);
	rotation_angle = fz_atof(rotation_angle_att);
	is_large_arc = !strcmp(is_large_arc_att, "true");
	is_clockwise = !strcmp(sweep_direction_att, "Clockwise");

	if (stroking && !is_stroked)
	{
		fz_moveto(doc, path, point_x, point_y);
		return;
	}

	xps_draw_arc(doc, path, size_x, size_y, rotation_angle, is_large_arc, is_clockwise, point_x, point_y);
}