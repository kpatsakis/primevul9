jpeg_parse_exif (JpegExifContext *context, j_decompress_ptr cinfo)
{
	jpeg_saved_marker_ptr cmarker;

	/* check for interesting Exif markers */
	cmarker = cinfo->marker_list;
	while (cmarker != NULL) {
		if (cmarker->marker == JPEG_APP0+1)
			jpeg_parse_exif_app1 (context, cmarker);
		else if (cmarker->marker == JPEG_APP0+2)
			jpeg_parse_exif_app2_segment (context, cmarker);
		cmarker = cmarker->next;
	}
}