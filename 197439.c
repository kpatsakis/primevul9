gdk_pixbuf__gif_image_stop_load (gpointer data, GError **error)
{
	GifContext *context = (GifContext *) data;
        gboolean retval = TRUE;
        
        if (context->state != GIF_DONE || context->animation->frames == NULL) {
                g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                                     _("GIF image was truncated or incomplete."));

                retval = FALSE;
        }
        
        g_object_unref (context->animation);

  	g_free (context->buf);
	g_free (context);

        return retval;
}