_jpeg_error_exit (j_common_ptr error)
{
	j_decompress_ptr cinfo = (j_decompress_ptr)error;
	struct _jpeg_src_mgr *src = (struct _jpeg_src_mgr *)cinfo->src;

	longjmp (src->setjmp_buffer, 1);
}