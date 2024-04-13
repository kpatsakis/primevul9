xps_parse_element(xps_document *doc, const fz_matrix *ctm, const fz_rect *area, char *base_uri, xps_resource *dict, fz_xml *node)
{
	if (doc->cookie && doc->cookie->abort)
		return;
	if (!strcmp(fz_xml_tag(node), "Path"))
		xps_parse_path(doc, ctm, base_uri, dict, node);
	if (!strcmp(fz_xml_tag(node), "Glyphs"))
		xps_parse_glyphs(doc, ctm, base_uri, dict, node);
	if (!strcmp(fz_xml_tag(node), "Canvas"))
		xps_parse_canvas(doc, ctm, area, base_uri, dict, node);
	if (!strcmp(fz_xml_tag(node), "mc:AlternateContent"))
	{
		node = xps_lookup_alternate_content(node);
		if (node)
			xps_parse_element(doc, ctm, area, base_uri, dict, node);
	}
	/* skip unknown tags (like Foo.Resources and similar) */
}