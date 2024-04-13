stdio_init_source (j_decompress_ptr cinfo)
{
  stdio_src_ptr src = (stdio_src_ptr)cinfo->src;
  src->start_of_file = FALSE;
}