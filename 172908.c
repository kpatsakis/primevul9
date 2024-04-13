jinit_read_bmp (j_compress_ptr cinfo, boolean use_inversion_array)
{
  bmp_source_ptr source;

  /* Create module interface object */
  source = (bmp_source_ptr)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
                                  sizeof(bmp_source_struct));
  source->cinfo = cinfo;        /* make back link for subroutines */
  /* Fill in method ptrs, except get_pixel_rows which start_input sets */
  source->pub.start_input = start_input_bmp;
  source->pub.finish_input = finish_input_bmp;

  source->use_inversion_array = use_inversion_array;

  return (cjpeg_source_ptr) source;
}