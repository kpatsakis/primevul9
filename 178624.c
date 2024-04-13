gdk_pixbuf__jpeg_image_save_to_callback (GdkPixbufSaveFunc   save_func,
					 gpointer            user_data,
					 GdkPixbuf          *pixbuf, 
					 gchar             **keys,
					 gchar             **values,
					 GError            **error)
{
	return real_save_jpeg (pixbuf, keys, values, error,
			       TRUE, NULL, save_func, user_data);
}