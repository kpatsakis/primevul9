xps_parse_path_geometry(xps_document *doc, xps_resource *dict, fz_xml *root, int stroking, int *fill_rule)
{
	fz_xml *node;

	char *figures_att;
	char *fill_rule_att;
	char *transform_att;

	fz_xml *transform_tag = NULL;
	fz_xml *figures_tag = NULL; /* only used by resource */

	fz_matrix transform;
	fz_path *path;

	figures_att = fz_xml_att(root, "Figures");
	fill_rule_att = fz_xml_att(root, "FillRule");
	transform_att = fz_xml_att(root, "Transform");

	for (node = fz_xml_down(root); node; node = fz_xml_next(node))
	{
		if (!strcmp(fz_xml_tag(node), "PathGeometry.Transform"))
			transform_tag = fz_xml_down(node);
	}

	xps_resolve_resource_reference(doc, dict, &transform_att, &transform_tag, NULL);
	xps_resolve_resource_reference(doc, dict, &figures_att, &figures_tag, NULL);

	if (fill_rule_att)
	{
		if (!strcmp(fill_rule_att, "NonZero"))
			*fill_rule = 1;
		if (!strcmp(fill_rule_att, "EvenOdd"))
			*fill_rule = 0;
	}

	transform = fz_identity;
	if (transform_att)
		xps_parse_render_transform(doc, transform_att, &transform);
	if (transform_tag)
		xps_parse_matrix_transform(doc, transform_tag, &transform);

	if (figures_att)
		path = xps_parse_abbreviated_geometry(doc, figures_att, fill_rule);
	else
		path = fz_new_path(doc->ctx);

	if (figures_tag)
		xps_parse_path_figure(doc->ctx, path, figures_tag, stroking);

	for (node = fz_xml_down(root); node; node = fz_xml_next(node))
	{
		if (!strcmp(fz_xml_tag(node), "PathFigure"))
			xps_parse_path_figure(doc->ctx, path, node, stroking);
	}

	if (transform_att || transform_tag)
		fz_transform_path(doc->ctx, path, &transform);

	return path;
}