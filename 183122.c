set_gif_lzw_clear_code (GifContext *context)
{
	context->state = GIF_LZW_CLEAR_CODE;
	context->lzw_code_pending = -1;
}