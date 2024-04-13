_jpeg_skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
	struct _jpeg_src_mgr *src = (struct _jpeg_src_mgr *)cinfo->src;

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			_jpeg_fill_input_buffer (cinfo);
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}