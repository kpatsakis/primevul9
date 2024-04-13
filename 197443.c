gif_main_loop (GifContext *context)
{
	gint retval = 0;

	do {
		switch (context->state) {
		case GIF_START:
                        LOG("start\n");
			retval = gif_init (context);
			break;

		case GIF_GET_COLORMAP:
                        LOG("get_colormap\n");
			retval = gif_get_colormap (context);
			if (retval == 0)
				context->state = GIF_GET_NEXT_STEP;
			break;

		case GIF_GET_NEXT_STEP:
                        LOG("next_step\n");
			retval = gif_get_next_step (context);
			break;

		case GIF_GET_FRAME_INFO:
                        LOG("frame_info\n");
			retval = gif_get_frame_info (context);
			break;

		case GIF_GET_EXTENSION:
                        LOG("get_extension\n");
			retval = gif_get_extension (context);
			if (retval == 0)
				context->state = GIF_GET_NEXT_STEP;
			break;

		case GIF_GET_COLORMAP2:
                        LOG("get_colormap2\n");
			retval = gif_get_colormap2 (context);
			if (retval == 0)
				gif_set_prepare_lzw (context);
			break;

		case GIF_PREPARE_LZW:
                        LOG("prepare_lzw\n");
			retval = gif_prepare_lzw (context);
			break;

		case GIF_LZW_FILL_BUFFER:
                        LOG("fill_buffer\n");
			retval = gif_lzw_fill_buffer (context);
			break;

		case GIF_LZW_CLEAR_CODE:
                        LOG("clear_code\n");
			retval = gif_lzw_clear_code (context);
			break;

		case GIF_GET_LZW:
                        LOG("get_lzw\n");
			retval = gif_get_lzw (context);
			break;

		case GIF_DONE:
                        LOG("done\n");
		default:
			retval = 0;
			goto done;
		};
	} while ((retval == 0) || (retval == -3));
 done:
	return retval;
}