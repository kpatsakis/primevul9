gif_set_prepare_lzw (GifContext *context)
{
	context->state = GIF_PREPARE_LZW;
	context->lzw_code_pending = -1;
}