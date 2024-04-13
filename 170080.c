static gboolean gdk_pixbuf__bmp_image_stop_load(gpointer data, GError **error)
{
	gboolean retval = TRUE;
	
	struct bmp_progressive_state *context =
	    (struct bmp_progressive_state *) data;

        /* FIXME this thing needs to report errors if
         * we have unused image data
         */
	
	g_return_val_if_fail(context != NULL, TRUE);

	g_free(context->Colormap);

	if (context->pixbuf)
		g_object_unref(context->pixbuf);

	if (context->read_state == READ_STATE_HEADERS) {
                if (error && *error == NULL) {
                        g_set_error_literal (error,
                                             GDK_PIXBUF_ERROR,
                                             GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                                             _("Premature end-of-file encountered"));
                }
		retval = FALSE;
	}
	
	g_free(context->buff);
	g_free(context);

        return retval;
}