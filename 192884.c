png_icc_set_sRGB(png_const_structrp png_ptr,
    png_colorspacerp colorspace, png_const_bytep profile, uLong adler)
{
   /* Is this profile one of the known ICC sRGB profiles?  If it is, just set
    * the sRGB information.
    */
   if (png_compare_ICC_profile_with_sRGB(png_ptr, profile, adler) != 0)
      (void)png_colorspace_set_sRGB(png_ptr, colorspace,
         (int)/*already checked*/png_get_uint_32(profile+64));
}