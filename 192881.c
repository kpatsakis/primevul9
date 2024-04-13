png_colorspace_set_ICC(png_const_structrp png_ptr, png_colorspacerp colorspace,
    png_const_charp name, png_uint_32 profile_length, png_const_bytep profile,
    int color_type)
{
   if ((colorspace->flags & PNG_COLORSPACE_INVALID) != 0)
      return 0;

   if (icc_check_length(png_ptr, colorspace, name, profile_length) != 0 &&
       png_icc_check_header(png_ptr, colorspace, name, profile_length, profile,
           color_type) != 0 &&
       png_icc_check_tag_table(png_ptr, colorspace, name, profile_length,
           profile) != 0)
   {
#     if defined(PNG_sRGB_SUPPORTED) && PNG_sRGB_PROFILE_CHECKS >= 0
         /* If no sRGB support, don't try storing sRGB information */
         png_icc_set_sRGB(png_ptr, colorspace, profile, 0);
#     endif
      return 1;
   }

   /* Failure case */
   return 0;
}