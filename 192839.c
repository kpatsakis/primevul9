png_get_libpng_ver(png_const_structrp png_ptr)
{
   /* Version of *.c files used when building libpng */
   return png_get_header_ver(png_ptr);
}