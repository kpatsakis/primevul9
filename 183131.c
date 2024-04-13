gif_lzw_fill_buffer (GifContext *context)
{
	gint retval;

	if (context->code_done) {
		if (context->code_curbit >= context->code_lastbit) {
                        g_set_error_literal (context->error,
                                             GDK_PIXBUF_ERROR,
                                             GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                                             _("GIF file was missing some data (perhaps it was truncated somehow?)"));

			return -2;
		}
                /* Is this supposed to be an error or what? */
		/* g_message ("trying to read more data after we've done stuff\n"); */
                g_set_error (context->error,
                             GDK_PIXBUF_ERROR,
                             GDK_PIXBUF_ERROR_FAILED,
                             _("Internal error in the GIF loader (%s)"),
                             G_STRLOC);
                
		return -2;
	}

	context->block_buf[0] = context->block_buf[context->code_last_byte - 2];
	context->block_buf[1] = context->block_buf[context->code_last_byte - 1];

	retval = get_data_block (context, &context->block_buf[2], NULL);

	if (retval == -1)
		return -1;

	if (context->block_count == 0)
		context->code_done = TRUE;

	context->code_last_byte = 2 + context->block_count;
	context->code_curbit = (context->code_curbit - context->code_lastbit) + 16;
	context->code_lastbit = (2 + context->block_count) * 8;

	context->state = context->old_state;
	return 0;
}