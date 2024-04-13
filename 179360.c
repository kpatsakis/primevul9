fill_tiff_error (GError     **error,
		 const gchar *image_uri)
{
	if (_tiff_error) {
		g_set_error (error,
			     GXPS_ERROR,
			     GXPS_ERROR_IMAGE,
			     "Error loading TIFF image %s: %s",
			     image_uri, _tiff_error);
		g_free (_tiff_error);
		_tiff_error = NULL;
	} else {
		g_set_error (error,
			     GXPS_ERROR,
			     GXPS_ERROR_IMAGE,
			     "Error loading TIFF image %s",
			     image_uri);
	}
}