gdk_pixbuf__jpeg_image_load (FILE *f, GError **error)
{
	gint   i;
	char   otag_str[5];
	char  *density_str;
	GdkPixbuf * volatile pixbuf = NULL;
	guchar *dptr;
	guchar *lines[4]; /* Used to expand rows, via rec_outbuf_height, 
                           * from the header file: 
                           * " Usually rec_outbuf_height will be 1 or 2, 
                           * at most 4."
			   */
	guchar **lptr;
	struct jpeg_decompress_struct cinfo;
	struct error_handler_data jerr;
	stdio_src_ptr src;
	gchar *icc_profile_base64;
	gchar *comment;
	JpegExifContext exif_context = { 0, };

	/* setup error handler */
	cinfo.err = jpeg_std_error (&jerr.pub);
	jerr.pub.error_exit = fatal_error_handler;
        jerr.pub.output_message = output_message_handler;
        jerr.error = error;
        
	if (sigsetjmp (jerr.setjmp_buffer, 1)) {
		/* Whoops there was a jpeg error */
		if (pixbuf)
			g_object_unref (pixbuf);

		jpeg_destroy_decompress (&cinfo);
		jpeg_destroy_exif_context (&exif_context);

		/* error should have been set by fatal_error_handler () */
		return NULL;
	}

	/* load header, setup */
	jpeg_create_decompress (&cinfo);

	cinfo.src = (struct jpeg_source_mgr *)
	  (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT,
				  sizeof (stdio_source_mgr));
	src = (stdio_src_ptr) cinfo.src;
	src->buffer = (JOCTET *)
	  (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT,
				      JPEG_PROG_BUF_SIZE * sizeof (JOCTET));

	src->pub.init_source = stdio_init_source;
	src->pub.fill_input_buffer = stdio_fill_input_buffer;
	src->pub.skip_input_data = stdio_skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
	src->pub.term_source = stdio_term_source;
	src->infile = f;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */

	jpeg_save_markers (&cinfo, JPEG_APP0+1, 0xffff);
	jpeg_save_markers (&cinfo, JPEG_APP0+2, 0xffff);
	jpeg_save_markers (&cinfo, JPEG_COM, 0xffff);
	jpeg_read_header (&cinfo, TRUE);

	/* parse exif data */
	jpeg_parse_exif (&exif_context, &cinfo);
	
	jpeg_start_decompress (&cinfo);
	cinfo.do_fancy_upsampling = FALSE;
	cinfo.do_block_smoothing = FALSE;

	pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, 
				 cinfo.out_color_components == 4 ? TRUE : FALSE, 
				 8, cinfo.output_width, cinfo.output_height);
	      
	if (!pixbuf) {
                /* broken check for *error == NULL for robustness against
                 * crappy JPEG library
                 */
                if (error && *error == NULL) {
                        g_set_error_literal (error,
                                             GDK_PIXBUF_ERROR,
                                             GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY,
                                             _("Insufficient memory to load image, try exiting some applications to free memory"));
                }
               
		goto out; 
	}

	comment = jpeg_get_comment (&cinfo);
	if (comment != NULL) {
		gdk_pixbuf_set_option (pixbuf, "comment", comment);
		g_free (comment);
	}

	switch (cinfo.density_unit) {
	case 1:
		/* Dots per inch (no conversion required) */
		density_str = g_strdup_printf ("%d", cinfo.X_density);
		gdk_pixbuf_set_option (pixbuf, "x-dpi", density_str);
		g_free (density_str);
		density_str = g_strdup_printf ("%d", cinfo.Y_density);
		gdk_pixbuf_set_option (pixbuf, "y-dpi", density_str);
		g_free (density_str);
		break;
	case 2:
		/* Dots per cm - convert into dpi */
		density_str = g_strdup_printf ("%d", DPCM_TO_DPI (cinfo.X_density));
		gdk_pixbuf_set_option (pixbuf, "x-dpi", density_str);
		g_free (density_str);
		density_str = g_strdup_printf ("%d", DPCM_TO_DPI (cinfo.Y_density));
		gdk_pixbuf_set_option (pixbuf, "y-dpi", density_str);
		g_free (density_str);
		break;
	}

	/* if orientation tag was found */
	if (exif_context.orientation != 0) {
		g_snprintf (otag_str, sizeof (otag_str), "%d", exif_context.orientation);
		gdk_pixbuf_set_option (pixbuf, "orientation", otag_str);
	}

	/* if icc profile was found */
	if (exif_context.icc_profile != NULL) {
		icc_profile_base64 = g_base64_encode ((const guchar *) exif_context.icc_profile, exif_context.icc_profile_size);
		gdk_pixbuf_set_option (pixbuf, "icc-profile", icc_profile_base64);
		g_free (icc_profile_base64);
	}

	dptr = pixbuf->pixels;

	/* decompress all the lines, a few at a time */
	while (cinfo.output_scanline < cinfo.output_height) {
		lptr = lines;
		for (i = 0; i < cinfo.rec_outbuf_height; i++) {
			*lptr++ = dptr;
			dptr += pixbuf->rowstride;
		}

		jpeg_read_scanlines (&cinfo, lines, cinfo.rec_outbuf_height);

		switch (cinfo.out_color_space) {
		    case JCS_GRAYSCALE:
		      explode_gray_into_buf (&cinfo, lines);
		      break;
		    case JCS_RGB:
		      /* do nothing */
		      break;
		    case JCS_CMYK:
		      convert_cmyk_to_rgb (&cinfo, lines);
		      break;
		    default:
		      g_object_unref (pixbuf);
		      pixbuf = NULL;
		      if (error && *error == NULL) {
                        g_set_error (error,
                                     GDK_PIXBUF_ERROR,
				     GDK_PIXBUF_ERROR_UNKNOWN_TYPE,
				     _("Unsupported JPEG color space (%s)"),
				     colorspace_name (cinfo.out_color_space)); 
		      }
               	      goto out; 
		}
	}

out:
	jpeg_finish_decompress (&cinfo);
	jpeg_destroy_decompress (&cinfo);
	jpeg_destroy_exif_context (&exif_context);

	return pixbuf;
}