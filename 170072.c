gdk_pixbuf__bmp_image_save (FILE          *f, 
                            GdkPixbuf     *pixbuf, 
                            gchar        **keys,
                            gchar        **values,
                            GError       **error)
{
	return gdk_pixbuf__bmp_image_save_to_callback (save_to_file_cb,
						       f, pixbuf, keys,
						       values, error);
}