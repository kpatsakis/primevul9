png_get_header_ver(png_const_structrp png_ptr)
{
   /* Version of *.h files used when building libpng */
   PNG_UNUSED(png_ptr)  /* Silence compiler warning about unused png_ptr */
   return PNG_LIBPNG_VER_STRING;
}