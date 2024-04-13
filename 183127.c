gif_lzw_clear_code (GifContext *context)
{
	gint code;

	code = get_code (context, context->lzw_code_size);
	if (code == -3)
		return -0;

	context->lzw_firstcode = context->lzw_oldcode = code;
	context->lzw_code_pending = code;
	context->state = GIF_GET_LZW;
	return 0;
}