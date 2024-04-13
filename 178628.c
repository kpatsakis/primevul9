jpeg_destroy_exif_context (JpegExifContext *context)
{
	g_free (context->icc_profile);
}