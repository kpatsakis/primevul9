png_icc_check_length(png_const_structrp png_ptr, png_colorspacerp colorspace,
    png_const_charp name, png_uint_32 profile_length)
{
   if (!icc_check_length(png_ptr, colorspace, name, profile_length))
      return 0;

   /* This needs to be here because the 'normal' check is in
    * png_decompress_chunk, yet this happens after the attempt to
    * png_malloc_base the required data.  We only need this on read; on write
    * the caller supplies the profile buffer so libpng doesn't allocate it.  See
    * the call to icc_check_length below (the write case).
    */
#  ifdef PNG_SET_USER_LIMITS_SUPPORTED
      else if (png_ptr->user_chunk_malloc_max > 0 &&
               png_ptr->user_chunk_malloc_max < profile_length)
         return png_icc_profile_error(png_ptr, colorspace, name, profile_length,
             "exceeds application limits");
#  elif PNG_USER_CHUNK_MALLOC_MAX > 0
      else if (PNG_USER_CHUNK_MALLOC_MAX < profile_length)
         return png_icc_profile_error(png_ptr, colorspace, name, profile_length,
             "exceeds libpng limits");
#  else /* !SET_USER_LIMITS */
      /* This will get compiled out on all 32-bit and better systems. */
      else if (PNG_SIZE_MAX < profile_length)
         return png_icc_profile_error(png_ptr, colorspace, name, profile_length,
             "exceeds system limits");
#  endif /* !SET_USER_LIMITS */

   return 1;
}