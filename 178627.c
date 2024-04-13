real_save_jpeg (GdkPixbuf          *pixbuf,
		gchar             **keys,
		gchar             **values,
		GError            **error,
		gboolean            to_callback,
		FILE               *f,
		GdkPixbufSaveFunc   save_func,
		gpointer            user_data)
{
        /* FIXME error handling is broken */
        
       struct jpeg_compress_struct cinfo;
       guchar *buf = NULL;
       guchar *ptr;
       guchar *pixels = NULL;
       JSAMPROW *jbuf;
       int y = 0;
       volatile int quality = 75; /* default; must be between 0 and 100 */
       int i, j;
       int w, h = 0;
       int rowstride = 0;
       int n_channels;
       struct error_handler_data jerr;
       ToFunctionDestinationManager to_callback_destmgr;
       int x_density = 0;
       int y_density = 0;
       gchar *icc_profile = NULL;
       gchar *data;
       gint retval = TRUE;
       gsize icc_profile_size = 0;

       to_callback_destmgr.buffer = NULL;

       if (keys && *keys) {
               gchar **kiter = keys;
               gchar **viter = values;

               while (*kiter) {
                       if (strcmp (*kiter, "quality") == 0) {
                               char *endptr = NULL;
                               quality = strtol (*viter, &endptr, 10);

                               if (endptr == *viter) {
                                       g_set_error (error,
                                                    GDK_PIXBUF_ERROR,
                                                    GDK_PIXBUF_ERROR_BAD_OPTION,
                                                    _("JPEG quality must be a value between 0 and 100; value '%s' could not be parsed."),
                                                    *viter);

                                       retval = FALSE;
                                       goto cleanup;
                               }
                               
                               if (quality < 0 ||
                                   quality > 100) {
                                       /* This is a user-visible error;
                                        * lets people skip the range-checking
                                        * in their app.
                                        */
                                       g_set_error (error,
                                                    GDK_PIXBUF_ERROR,
                                                    GDK_PIXBUF_ERROR_BAD_OPTION,
                                                    _("JPEG quality must be a value between 0 and 100; value '%d' is not allowed."),
                                                    quality);

                                       retval = FALSE;
                                       goto cleanup;
                               }
                       } else if (strcmp (*kiter, "x-dpi") == 0) {
                               char *endptr = NULL;
                               x_density = strtol (*viter, &endptr, 10);
                               if (endptr == *viter)
                                       x_density = -1;

                               if (x_density <= 0 ||
                                   x_density > 65535) {
                                       /* This is a user-visible error;
                                        * lets people skip the range-checking
                                        * in their app.
                                        */
                                       g_set_error (error,
                                                    GDK_PIXBUF_ERROR,
                                                    GDK_PIXBUF_ERROR_BAD_OPTION,
                                                    _("JPEG x-dpi must be a value between 1 and 65535; value '%s' is not allowed."),
                                                    *viter);

                                       retval = FALSE;
                                       goto cleanup;
                               }
                       } else if (strcmp (*kiter, "y-dpi") == 0) {
                               char *endptr = NULL;
                               y_density = strtol (*viter, &endptr, 10);
                               if (endptr == *viter)
                                       y_density = -1;

                               if (y_density <= 0 ||
                                   y_density > 65535) {
                                       /* This is a user-visible error;
                                        * lets people skip the range-checking
                                        * in their app.
                                        */
                                       g_set_error (error,
                                                    GDK_PIXBUF_ERROR,
                                                    GDK_PIXBUF_ERROR_BAD_OPTION,
                                                    _("JPEG y-dpi must be a value between 1 and 65535; value '%s' is not allowed."),
                                                    *viter);

                                       retval = FALSE;
                                       goto cleanup;
                               }
                       } else if (strcmp (*kiter, "icc-profile") == 0) {
                               /* decode from base64 */
                               icc_profile = (gchar*) g_base64_decode (*viter, &icc_profile_size);
                               if (icc_profile_size < 127) {
                                       /* This is a user-visible error */
                                       g_set_error (error,
                                                    GDK_PIXBUF_ERROR,
                                                    GDK_PIXBUF_ERROR_BAD_OPTION,
                                                    _("Color profile has invalid length '%u'."),
                                                    (guint) icc_profile_size);
                                       retval = FALSE;
                                       goto cleanup;
                               }
                       } else {
                               g_warning ("Unrecognized parameter (%s) passed to JPEG saver.", *kiter);
                       }
               
                       ++kiter;
                       ++viter;
               }
       }
       
       rowstride = gdk_pixbuf_get_rowstride (pixbuf);
       n_channels = gdk_pixbuf_get_n_channels (pixbuf);

       w = gdk_pixbuf_get_width (pixbuf);
       h = gdk_pixbuf_get_height (pixbuf);
       pixels = gdk_pixbuf_get_pixels (pixbuf);

       /* Guaranteed by the caller. */
       g_assert (w >= 0);
       g_assert (h >= 0);
       g_assert (rowstride >= 0);
       g_assert (n_channels >= 0);

       /* Allocate a small buffer to convert image data,
	* and a larger buffer if doing to_callback save.
	*/
       buf = g_try_malloc (w * 3 * sizeof (guchar));
       if (!buf) {
	       g_set_error_literal (error,
                                    GDK_PIXBUF_ERROR,
                                    GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY,
                                    _("Couldn't allocate memory for loading JPEG file"));
	       retval = FALSE;
	       goto cleanup;
       }
       if (to_callback) {
	       to_callback_destmgr.buffer = g_try_malloc (TO_FUNCTION_BUF_SIZE);
	       if (!to_callback_destmgr.buffer) {
		       g_set_error_literal (error,
                                            GDK_PIXBUF_ERROR,
                                            GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY,
                                            _("Couldn't allocate memory for loading JPEG file"));
		       retval = FALSE;
		       goto cleanup;
	       }
       }

       /* set up error handling */
       cinfo.err = jpeg_std_error (&(jerr.pub));
       jerr.pub.error_exit = fatal_error_handler;
       jerr.pub.output_message = output_message_handler;
       jerr.error = error;
       
       if (sigsetjmp (jerr.setjmp_buffer, 1)) {
               jpeg_destroy_compress (&cinfo);
	       retval = FALSE;
	       goto cleanup;
       }

       /* setup compress params */
       jpeg_create_compress (&cinfo);
       if (to_callback) {
	       to_callback_destmgr.pub.init_destination    = to_callback_init;
	       to_callback_destmgr.pub.empty_output_buffer = to_callback_empty_output_buffer;
	       to_callback_destmgr.pub.term_destination    = to_callback_terminate;
	       to_callback_destmgr.error = error;
	       to_callback_destmgr.save_func = save_func;
	       to_callback_destmgr.user_data = user_data;
	       cinfo.dest = (struct jpeg_destination_mgr*) &to_callback_destmgr;
       } else {
	       jpeg_stdio_dest (&cinfo, f);
       }
       cinfo.image_width      = w;
       cinfo.image_height     = h;
       cinfo.input_components = 3; 
       cinfo.in_color_space   = JCS_RGB;

       /* set up jepg compression parameters */
       jpeg_set_defaults (&cinfo);
       jpeg_set_quality (&cinfo, quality, TRUE);

       /* set density information */
       if (x_density > 0 && y_density > 0) {
           cinfo.density_unit = 1; /* Dots per inch */
           cinfo.X_density = x_density;
           cinfo.Y_density = y_density;
       }

       jpeg_start_compress (&cinfo, TRUE);

	/* write ICC profile data */
	if (icc_profile != NULL) {
		/* optimise for the common case where only one APP2 segment is required */
		if (icc_profile_size < 0xffef) {
			data = g_new (gchar, icc_profile_size + 14);
			memcpy (data, "ICC_PROFILE\000\001\001", 14);
			memcpy (data + 14, icc_profile, icc_profile_size);
			jpeg_write_marker (&cinfo, JPEG_APP0+2, (const JOCTET *) data, icc_profile_size + 14);
			g_free (data);
		} else {
			guint segments;
			guint size = 0xffef;
			guint offset;

			segments = (guint) ceilf ((gfloat) icc_profile_size / (gfloat) 0xffef);
			data = g_new (gchar, 0xffff);
			memcpy (data, "ICC_PROFILE\000", 12);
			data[13] = segments;
			for (i=0; i<=segments; i++) {
				data[12] = i;
				offset = 0xffef * i;

				/* last segment */
				if (i == segments)
					size = icc_profile_size % 0xffef;

				memcpy (data + 14, icc_profile + offset, size);
				jpeg_write_marker (&cinfo, JPEG_APP0+2, (const JOCTET *) data, size + 14);
			}
			g_free (data);
		}
	}

       /* get the start pointer */
       ptr = pixels;
       /* go one scanline at a time... and save */
       i = 0;
       while (cinfo.next_scanline < cinfo.image_height) {
               /* convert scanline from ARGB to RGB packed */
               for (j = 0; j < w; j++)
                       memcpy (&(buf[j*3]), &(ptr[(gsize)i*rowstride + j*n_channels]), 3);

               /* write scanline */
               jbuf = (JSAMPROW *)(&buf);
               if (jpeg_write_scanlines (&cinfo, jbuf, 1) == 0) {
                      jpeg_destroy_compress (&cinfo);
                      retval = FALSE;
                      goto cleanup;
               }

               i++;
               y++;

       }

       /* finish off */
       jpeg_finish_compress (&cinfo);
       jpeg_destroy_compress(&cinfo);
cleanup:
	g_free (buf);
	g_free (to_callback_destmgr.buffer);
	g_free (icc_profile);
	return retval;
}