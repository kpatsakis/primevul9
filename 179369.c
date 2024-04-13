gxps_images_create_from_jpeg (GXPSArchive *zip,
			      const gchar *image_uri,
			      GError     **error)
{
#ifdef HAVE_LIBJPEG
	GInputStream                 *stream;
	struct jpeg_error_mgr         error_mgr;
	struct jpeg_decompress_struct cinfo;
	struct _jpeg_src_mgr          src;
	GXPSImage                    *image;
	guchar                       *data;
	gint                          stride;
	JSAMPARRAY                    lines;
	gint                          jpeg_stride;
	gint                          i;

	stream = gxps_archive_open (zip, image_uri);
	if (!stream) {
		g_set_error (error,
			     GXPS_ERROR,
			     GXPS_ERROR_SOURCE_NOT_FOUND,
			     "Image source %s not found in archive",
			     image_uri);
		return NULL;
	}

	jpeg_std_error (&error_mgr);
	error_mgr.error_exit = _jpeg_error_exit;

	jpeg_create_decompress (&cinfo);
	cinfo.err = &error_mgr;

	src.stream = stream;
	src.buffer = (JOCTET *)	(*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, JPOOL_PERMANENT,
							   JPEG_PROG_BUF_SIZE * sizeof (JOCTET));

	src.pub.init_source = _jpeg_init_source;
	src.pub.fill_input_buffer = _jpeg_fill_input_buffer;
	src.pub.skip_input_data = _jpeg_skip_input_data;
	src.pub.resync_to_restart = jpeg_resync_to_restart;
	src.pub.term_source = _jpeg_term_source;
	src.pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src.pub.next_input_byte = NULL; /* until buffer loaded */
	cinfo.src = (struct jpeg_source_mgr *)&src;

	if (setjmp (src.setjmp_buffer)) {
		g_set_error (error,
			     GXPS_ERROR,
			     GXPS_ERROR_IMAGE,
			     "Error loading JPEG image %s",
			     image_uri);
		g_object_unref (stream);
		return NULL;
	}

	jpeg_read_header (&cinfo, TRUE);

	cinfo.do_fancy_upsampling = FALSE;
	jpeg_start_decompress (&cinfo);

	image = g_slice_new (GXPSImage);
	image->surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24,
						     cinfo.output_width,
						     cinfo.output_height);
	image->res_x = 96;
	image->res_y = 96;
	if (cairo_surface_status (image->surface)) {
		g_set_error (error,
			     GXPS_ERROR,
			     GXPS_ERROR_IMAGE,
			     "Error loading JPEG image %s: %s",
			     image_uri,
			     cairo_status_to_string (cairo_surface_status (image->surface)));
		jpeg_destroy_decompress (&cinfo);
		gxps_image_free (image);
		g_object_unref (stream);

		return NULL;
	}

	data = cairo_image_surface_get_data (image->surface);
	stride = cairo_image_surface_get_stride (image->surface);
	jpeg_stride = cinfo.output_width * cinfo.out_color_components;
	lines = cinfo.mem->alloc_sarray((j_common_ptr) &cinfo, JPOOL_IMAGE, jpeg_stride, 4);

	while (cinfo.output_scanline < cinfo.output_height) {
		gint n_lines, x;

		n_lines = jpeg_read_scanlines (&cinfo, lines, cinfo.rec_outbuf_height);
		for (i = 0; i < n_lines; i++) {
			JSAMPLE *line = lines[i];
			guchar  *p = data;

			for (x = 0; x < cinfo.output_width; x++) {
				switch (cinfo.out_color_space) {
				case JCS_RGB:
					p[0] = line[2];
					p[1] = line[1];
					p[2] = line[0];
					p[3] = 0xff;
					break;
                                case JCS_GRAYSCALE:
                                        p[0] = line[0];
                                        p[1] = line[0];
                                        p[2] = line[0];
                                        p[3] = 0xff;
                                        break;
				default:
					GXPS_DEBUG (g_message ("Unsupported jpeg color space %s",
                                                               _jpeg_color_space_name (cinfo.out_color_space)));

					gxps_image_free (image);
					jpeg_destroy_decompress (&cinfo);
					g_object_unref (stream);
					return NULL;
				}
				line += cinfo.out_color_components;
				p += 4;
			}

			data += stride;
		}
	}

	if (cinfo.density_unit == 1) { /* dots/inch */
		image->res_x = cinfo.X_density;
		image->res_y = cinfo.Y_density;
	} else if (cinfo.density_unit == 2) { /* dots/cm */
		image->res_x = cinfo.X_density * CENTIMETERS_PER_INCH;
		image->res_y = cinfo.Y_density * CENTIMETERS_PER_INCH;
	}

	jpeg_finish_decompress (&cinfo);
	jpeg_destroy_decompress (&cinfo);
	g_object_unref (stream);

	cairo_surface_mark_dirty (image->surface);

	if (cairo_surface_status (image->surface)) {
		g_set_error (error,
			     GXPS_ERROR,
			     GXPS_ERROR_IMAGE,
			     "Error loading JPEG image %s: %s",
			     image_uri,
			     cairo_status_to_string (cairo_surface_status (image->surface)));
		gxps_image_free (image);

		return NULL;
	}

	return image;
#else
	return NULL;
#endif /* HAVE_LIBJPEG */
}