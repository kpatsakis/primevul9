xps_parse_gradient_brush(xps_document *doc, const fz_matrix *ctm, const fz_rect *area,
	char *base_uri, xps_resource *dict, fz_xml *root,
	void (*draw)(xps_document *, const fz_matrix*, const fz_rect *, struct stop *, int, fz_xml *, int))
{
	fz_xml *node;

	char *opacity_att;
	char *spread_att;
	char *transform_att;

	fz_xml *transform_tag = NULL;
	fz_xml *stop_tag = NULL;

	struct stop stop_list[MAX_STOPS];
	int stop_count;
	fz_matrix transform;
	int spread_method;

	opacity_att = fz_xml_att(root, "Opacity");
	spread_att = fz_xml_att(root, "SpreadMethod");
	transform_att = fz_xml_att(root, "Transform");

	for (node = fz_xml_down(root); node; node = fz_xml_next(node))
	{
		if (!strcmp(fz_xml_tag(node), "LinearGradientBrush.Transform"))
			transform_tag = fz_xml_down(node);
		if (!strcmp(fz_xml_tag(node), "RadialGradientBrush.Transform"))
			transform_tag = fz_xml_down(node);
		if (!strcmp(fz_xml_tag(node), "LinearGradientBrush.GradientStops"))
			stop_tag = fz_xml_down(node);
		if (!strcmp(fz_xml_tag(node), "RadialGradientBrush.GradientStops"))
			stop_tag = fz_xml_down(node);
	}

	xps_resolve_resource_reference(doc, dict, &transform_att, &transform_tag, NULL);

	spread_method = SPREAD_PAD;
	if (spread_att)
	{
		if (!strcmp(spread_att, "Pad"))
			spread_method = SPREAD_PAD;
		if (!strcmp(spread_att, "Reflect"))
			spread_method = SPREAD_REFLECT;
		if (!strcmp(spread_att, "Repeat"))
			spread_method = SPREAD_REPEAT;
	}

	transform = fz_identity;
	if (transform_att)
		xps_parse_render_transform(doc, transform_att, &transform);
	if (transform_tag)
		xps_parse_matrix_transform(doc, transform_tag, &transform);
	fz_concat(&transform, &transform, ctm);

	if (!stop_tag) {
		fz_warn(doc->ctx, "missing gradient stops tag");
		return;
	}

	stop_count = xps_parse_gradient_stops(doc, base_uri, stop_tag, stop_list, MAX_STOPS);
	if (stop_count == 0)
	{
		fz_warn(doc->ctx, "no gradient stops found");
		return;
	}

	xps_begin_opacity(doc, &transform, area, base_uri, dict, opacity_att, NULL);

	draw(doc, &transform, area, stop_list, stop_count, root, spread_method);

	xps_end_opacity(doc, base_uri, dict, opacity_att, NULL);
}