xps_clip(xps_document *doc, const fz_matrix *ctm, xps_resource *dict, char *clip_att, fz_xml *clip_tag)
{
	fz_path *path;
	int fill_rule = 0;

	if (clip_att)
		path = xps_parse_abbreviated_geometry(doc, clip_att, &fill_rule);
	else if (clip_tag)
		path = xps_parse_path_geometry(doc, dict, clip_tag, 0, &fill_rule);
	else
		path = fz_new_path(doc->ctx);
	fz_clip_path(doc->dev, path, NULL, fill_rule == 0, ctm);
	fz_free_path(doc->ctx, path);
}