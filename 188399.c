xps_parse_radial_gradient_brush(xps_document *doc, const fz_matrix *ctm, const fz_rect *area,
	char *base_uri, xps_resource *dict, fz_xml *root)
{
	xps_parse_gradient_brush(doc, ctm, area, base_uri, dict, root, xps_draw_radial_gradient);
}