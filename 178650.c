jpeg_get_comment (j_decompress_ptr cinfo)
{
	jpeg_saved_marker_ptr cmarker;

	cmarker = cinfo->marker_list;
	while (cmarker != NULL) {
		if (cmarker->marker == JPEG_COM)
			return g_strndup ((const gchar *) cmarker->data, cmarker->data_length);
		cmarker = cmarker->next;
	}

	return NULL;
}