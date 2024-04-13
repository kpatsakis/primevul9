init_source (j_decompress_ptr cinfo)
{
	my_src_ptr src = (my_src_ptr) cinfo->src;

	src->skip_next = 0;
}