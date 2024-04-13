_jpeg_fill_input_buffer (j_decompress_ptr cinfo)
{
	struct _jpeg_src_mgr *src = (struct _jpeg_src_mgr *)cinfo->src;
	gssize num_bytes;

	num_bytes = g_input_stream_read (src->stream, src->buffer, JPEG_PROG_BUF_SIZE, NULL, NULL);
	if (num_bytes <= 0) {
		/* Insert a fake EOI marker */
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = num_bytes;

	return TRUE;
}