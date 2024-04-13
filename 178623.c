gdk_pixbuf__jpeg_image_save (FILE          *f, 
                             GdkPixbuf     *pixbuf, 
                             gchar        **keys,
                             gchar        **values,
                             GError       **error)
{
	return real_save_jpeg (pixbuf, keys, values, error,
			       FALSE, f, NULL, NULL);
}