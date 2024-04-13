xps_draw_linear_gradient(xps_document *doc, const fz_matrix *ctm, const fz_rect *area,
	struct stop *stops, int count,
	fz_xml *root, int spread)
{
	float x0, y0, x1, y1;
	int i, mi, ma;
	float dx, dy, x, y, k;
	fz_point p1, p2;
	fz_matrix inv;
	fz_rect local_area = *area;

	char *start_point_att = fz_xml_att(root, "StartPoint");
	char *end_point_att = fz_xml_att(root, "EndPoint");

	x0 = y0 = 0;
	x1 = y1 = 1;

	if (start_point_att)
		xps_parse_point(start_point_att, &x0, &y0);
	if (end_point_att)
		xps_parse_point(end_point_att, &x1, &y1);

	p1.x = x0; p1.y = y0; p2.x = x1; p2.y = y1;
	fz_transform_rect(&local_area, fz_invert_matrix(&inv, ctm));
	x = p2.x - p1.x; y = p2.y - p1.y;
	k = ((local_area.x0 - p1.x) * x + (local_area.y0 - p1.y) * y) / (x * x + y * y);
	mi = floorf(k); ma = ceilf(k);
	k = ((local_area.x1 - p1.x) * x + (local_area.y0 - p1.y) * y) / (x * x + y * y);
	mi = fz_mini(mi, floorf(k)); ma = fz_maxi(ma, ceilf(k));
	k = ((local_area.x0 - p1.x) * x + (local_area.y1 - p1.y) * y) / (x * x + y * y);
	mi = fz_mini(mi, floorf(k)); ma = fz_maxi(ma, ceilf(k));
	k = ((local_area.x1 - p1.x) * x + (local_area.y1 - p1.y) * y) / (x * x + y * y);
	mi = fz_mini(mi, floorf(k)); ma = fz_maxi(ma, ceilf(k));
	dx = x1 - x0; dy = y1 - y0;

	if (spread == SPREAD_REPEAT)
	{
		for (i = mi; i < ma; i++)
			xps_draw_one_linear_gradient(doc, ctm, stops, count, 0, x0 + i * dx, y0 + i * dy, x1 + i * dx, y1 + i * dy);
	}
	else if (spread == SPREAD_REFLECT)
	{
		if ((mi % 2) != 0)
			mi--;
		for (i = mi; i < ma; i += 2)
		{
			xps_draw_one_linear_gradient(doc, ctm, stops, count, 0, x0 + i * dx, y0 + i * dy, x1 + i * dx, y1 + i * dy);
			xps_draw_one_linear_gradient(doc, ctm, stops, count, 0, x0 + (i + 2) * dx, y0 + (i + 2) * dy, x1 + i * dx, y1 + i * dy);
		}
	}
	else
	{
		xps_draw_one_linear_gradient(doc, ctm, stops, count, 1, x0, y0, x1, y1);
	}
}