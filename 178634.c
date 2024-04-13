gdk_pixbuf__jpeg_image_load_lines (JpegProgContext  *context,
                                   GError          **error)
{
        struct jpeg_decompress_struct *cinfo = &context->cinfo;
        guchar *lines[4];
        guchar **lptr;
        guchar *rowptr;
        gint   nlines, i;

        /* keep going until we've done all scanlines */
        while (cinfo->output_scanline < cinfo->output_height) {
                lptr = lines;
                rowptr = context->dptr;
                for (i=0; i < cinfo->rec_outbuf_height; i++) {
                        *lptr++ = rowptr;
                        rowptr += context->pixbuf->rowstride;
                }

                nlines = jpeg_read_scanlines (cinfo, lines,
                                              cinfo->rec_outbuf_height);
                if (nlines == 0)
                        break;

                switch (cinfo->out_color_space) {
                case JCS_GRAYSCALE:
                        explode_gray_into_buf (cinfo, lines);
                        break;
                case JCS_RGB:
                        /* do nothing */
                        break;
                case JCS_CMYK:
                        convert_cmyk_to_rgb (cinfo, lines);
                        break;
                default:
                        if (error && *error == NULL) {
                                g_set_error (error,
                                             GDK_PIXBUF_ERROR,
                                             GDK_PIXBUF_ERROR_UNKNOWN_TYPE,
                                             _("Unsupported JPEG color space (%s)"),
                                             colorspace_name (cinfo->out_color_space));
                        }

                        return FALSE;
                }

                context->dptr += (gsize)nlines * context->pixbuf->rowstride;

                /* send updated signal */
		if (context->updated_func)
			(* context->updated_func) (context->pixbuf,
						   0,
						   cinfo->output_scanline - 1,
						   cinfo->image_width,
						   nlines,
						   context->user_data);
        }

        return TRUE;
}