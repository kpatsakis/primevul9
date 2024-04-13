save_to_file_cb (const gchar *buf,
		 gsize count,
		 GError **error,
		 gpointer data)
{
	gint bytes;
	
	while (count > 0) {
		bytes = fwrite (buf, sizeof (gchar), count, (FILE *) data);
		if (bytes <= 0)
			break;
		count -= bytes;
		buf += bytes;
	}

	if (count) {
		g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_FAILED,
                                     _("Couldn't write to TIFF file"));
		return FALSE;
	}
	
	return TRUE;
}