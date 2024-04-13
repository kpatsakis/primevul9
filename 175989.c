gdk_pixbuf__tiff_image_load_increment (gpointer data, const guchar *buf,
                                       guint size, GError **error)
{
	TiffContext *context = (TiffContext *) data;
        
	g_return_val_if_fail (data != NULL, FALSE);
        
        tiff_set_handlers ();

        if (!make_available_at_least (context, size)) {
                g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY,
                                     _("Insufficient memory to open TIFF file"));
                return FALSE;
        }
        
        memcpy (context->buffer + context->used, buf, size);
        context->used += size;
	return TRUE;
}