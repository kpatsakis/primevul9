xps_draw_radial_gradient(xps_document *doc, const fz_matrix *ctm, const fz_rect *area,
	struct stop *stops, int count,
	fz_xml *root, int spread)
{
	float x0, y0, r0;
	float x1, y1, r1;
	float xrad = 1;
	float yrad = 1;
	float invscale;
	int i, ma = 1;
	fz_matrix local_ctm = *ctm;
	fz_matrix inv;
	fz_rect local_area = *area;

	char *center_att = fz_xml_att(root, "Center");
	char *origin_att = fz_xml_att(root, "GradientOrigin");
	char *radius_x_att = fz_xml_att(root, "RadiusX");
	char *radius_y_att = fz_xml_att(root, "RadiusY");

	x0 = y0 = 0.0;
	x1 = y1 = 1.0;
	xrad = 1.0;
	yrad = 1.0;

	if (origin_att)
		xps_parse_point(origin_att, &x0, &y0);
	if (center_att)
		xps_parse_point(center_att, &x1, &y1);
	if (radius_x_att)
		xrad = fz_atof(radius_x_att);
	if (radius_y_att)
		yrad = fz_atof(radius_y_att);

	xrad = fz_max(0.01f, xrad);
	yrad = fz_max(0.01f, yrad);

	/* scale the ctm to make ellipses */
	if (fz_abs(xrad) > FLT_EPSILON)
	{
		fz_pre_scale(&local_ctm, 1, yrad/xrad);
	}

	if (yrad != 0.0)
	{
		invscale = xrad / yrad;
		y0 = y0 * invscale;
		y1 = y1 * invscale;
	}

	r0 = 0;
	r1 = xrad;

	fz_transform_rect(&local_area, fz_invert_matrix(&inv, &local_ctm));
	ma = fz_maxi(ma, ceilf(hypotf(local_area.x0 - x0, local_area.y0 - y0) / xrad));
	ma = fz_maxi(ma, ceilf(hypotf(local_area.x1 - x0, local_area.y0 - y0) / xrad));
	ma = fz_maxi(ma, ceilf(hypotf(local_area.x0 - x0, local_area.y1 - y0) / xrad));
	ma = fz_maxi(ma, ceilf(hypotf(local_area.x1 - x0, local_area.y1 - y0) / xrad));

	if (spread == SPREAD_REPEAT)
	{
		for (i = ma - 1; i >= 0; i--)
			xps_draw_one_radial_gradient(doc, &local_ctm, stops, count, 0, x0, y0, r0 + i * xrad, x1, y1, r1 + i * xrad);
	}
	else if (spread == SPREAD_REFLECT)
	{
		if ((ma % 2) != 0)
			ma++;
		for (i = ma - 2; i >= 0; i -= 2)
		{
			xps_draw_one_radial_gradient(doc, &local_ctm, stops, count, 0, x0, y0, r0 + i * xrad, x1, y1, r1 + i * xrad);
			xps_draw_one_radial_gradient(doc, &local_ctm, stops, count, 0, x0, y0, r0 + (i + 2) * xrad, x1, y1, r1 + i * xrad);
		}
	}
	else
	{
		xps_draw_one_radial_gradient(doc, &local_ctm, stops, count, 1, x0, y0, r0, x1, y1, r1);
	}
}