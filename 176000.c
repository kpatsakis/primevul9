gdk_pixbuf__tiff_image_load (FILE *f, GError **error)
{
        TIFF *tiff;
        int fd;
        GdkPixbuf *pixbuf;
        
        g_return_val_if_fail (f != NULL, NULL);

        tiff_set_handlers ();

        fd = fileno (f);

        /* On OSF, apparently fseek() works in some on-demand way, so
         * the fseek gdk_pixbuf_new_from_file() doesn't work here
         * since we are using the raw file descriptor. So, we call lseek() on the fd
         * before using it. (#60840)
         */
        lseek (fd, 0, SEEK_SET);
        tiff = TIFFFdOpen (fd, "libpixbuf-tiff", "r");

        if (!tiff) {
                g_set_error_literal (error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                                     _("Failed to open TIFF image"));
                return NULL;
        }

        pixbuf = tiff_image_parse (tiff, NULL, error);

        TIFFClose (tiff);

        return pixbuf;
}