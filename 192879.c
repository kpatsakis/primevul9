icc_check_length(png_const_structrp png_ptr, png_colorspacerp colorspace,
    png_const_charp name, png_uint_32 profile_length)
{
   if (profile_length < 132)
      return png_icc_profile_error(png_ptr, colorspace, name, profile_length,
          "too short");

   return 1;
}