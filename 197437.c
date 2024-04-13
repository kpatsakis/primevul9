gif_init (GifContext *context)
{
	unsigned char buf[16];
	char version[4];

	if (!gif_read (context, buf, 6)) {
		/* Unable to read magic number,
                 * gif_read() should have set error
                 */
		return -1;
	}

	if (strncmp ((char *) buf, "GIF", 3) != 0) {
		/* Not a GIF file */
                g_set_error_literal (context->error,
                                     GDK_PIXBUF_ERROR,
                                     GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                                     _("File does not appear to be a GIF file"));
		return -2;
	}

	strncpy (version, (char *) buf + 3, 3);
	version[3] = '\0';

	if ((strcmp (version, "87a") != 0) && (strcmp (version, "89a") != 0)) {
		/* bad version number, not '87a' or '89a' */
                g_set_error (context->error,
                             GDK_PIXBUF_ERROR,
                             GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                             _("Version %s of the GIF file format is not supported"),
                             version);
		return -2;
	}

	/* read the screen descriptor */
	if (!gif_read (context, buf, 7)) {
		/* Failed to read screen descriptor, error set */
		return -1;
	}

	context->width = LM_to_uint (buf[0], buf[1]);
	context->height = LM_to_uint (buf[2], buf[3]);
        /* The 4th byte is
         * high bit: whether to use the background index
         * next 3:   color resolution
         * next:     whether colormap is sorted by priority of allocation
         * last 3:   size of colormap
         */
	context->global_bit_pixel = 2 << (buf[4] & 0x07);
	context->global_color_resolution = (((buf[4] & 0x70) >> 3) + 1);
        context->has_global_cmap = (buf[4] & 0x80) != 0;
	context->background_index = buf[5];
	context->aspect_ratio = buf[6];

        /* Use background of transparent black as default, though if
         * one isn't set explicitly no one should ever use it.
         */
        context->animation->bg_red = 0;
        context->animation->bg_green = 0;
        context->animation->bg_blue = 0;

        context->animation->width = context->width;
        context->animation->height = context->height;

	if (context->has_global_cmap) {
		gif_set_get_colormap (context);
	} else {
		context->state = GIF_GET_NEXT_STEP;
	}

#ifdef DUMP_IMAGE_DETAILS
        g_print (">Image width: %d height: %d global_cmap: %d background: %d\n",
                 context->width, context->height, context->has_global_cmap, context->background_index);
#endif
        
	return 0;
}