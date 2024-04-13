xps_end_opacity(xps_document *doc, char *base_uri, xps_resource *dict,
	char *opacity_att, fz_xml *opacity_mask_tag)
{
	if (!opacity_att && !opacity_mask_tag)
		return;

	if (doc->opacity_top > 0)
		doc->opacity_top--;

	if (opacity_mask_tag)
	{
		if (strcmp(fz_xml_tag(opacity_mask_tag), "SolidColorBrush"))
			fz_pop_clip(doc->dev);
	}
}