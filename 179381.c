fill_png_error (GError      **error,
		const gchar  *image_uri,
		const gchar  *msg)
{
	if (msg) {
		g_set_error (error,
			     GXPS_ERROR,
			     GXPS_ERROR_IMAGE,
			     "Error loading PNG image %s: %s",
			     image_uri, msg);
	} else {
		g_set_error (error,
			     GXPS_ERROR,
			     GXPS_ERROR_IMAGE,
			     "Error loading PNG image %s",
			     image_uri);
	}
}