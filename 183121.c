gif_set_get_extension (GifContext *context)
{
	context->state = GIF_GET_EXTENSION;
	context->extension_flag = TRUE;
	context->extension_label = 0;
	context->block_count = 0;
	context->block_ptr = 0;
}