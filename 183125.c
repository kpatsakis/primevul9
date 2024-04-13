get_code (GifContext *context,
	  int   code_size)
{
	int i, j, ret;

	if ((context->code_curbit + code_size) >= context->code_lastbit){
		gif_set_lzw_fill_buffer (context);
		return -3;
	}

	ret = 0;
	for (i = context->code_curbit, j = 0; j < code_size; ++i, ++j)
		ret |= ((context->block_buf[i / 8] & (1 << (i % 8))) != 0) << j;

	context->code_curbit += code_size;

	return ret;
}