xps_parse_matrix_transform(xps_document *doc, fz_xml *root, fz_matrix *matrix)
{
	char *transform;

	*matrix = fz_identity;

	if (!strcmp(fz_xml_tag(root), "MatrixTransform"))
	{
		transform = fz_xml_att(root, "Matrix");
		if (transform)
			xps_parse_render_transform(doc, transform, matrix);
	}
}