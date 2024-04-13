gdk_pixbuf__tiff_image_stop_load (gpointer data,
                                  GError **error)
{
        TiffContext *context = data;
        TIFF *tiff;
        gboolean retval = FALSE;
        
        g_return_val_if_fail (data != NULL, FALSE);

        tiff_set_handlers ();

        tiff = TIFFClientOpen ("libtiff-pixbuf", "r", data, 
                               tiff_load_read, tiff_load_write, 
                               tiff_load_seek, tiff_load_close, 
                               tiff_load_size, 
                               tiff_load_map_file, tiff_load_unmap_file);
        if (!tiff) {
                g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_FAILED,
                                     _("Failed to load TIFF image"));
        } else {
                GdkPixbuf *pixbuf;
                
                pixbuf = tiff_image_parse (tiff, context, error);
                retval = (pixbuf != NULL);
                g_clear_object (&pixbuf);
                /* tiff_image_parse() can return NULL on success in a particular case */
                if (!retval && error && !*error) {
                        g_set_error_literal (error,
                                             GDK_PIXBUF_ERROR,
                                             GDK_PIXBUF_ERROR_FAILED,
                                             _("Failed to load TIFF image"));
                }
        }

        if (tiff)
                TIFFClose (tiff);

        g_free (context->buffer);
        g_free (context);

        return retval;
}