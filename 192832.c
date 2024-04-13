png_fixed(png_const_structrp png_ptr, double fp, png_const_charp text)
{
   double r = floor(100000 * fp + .5);

   if (r > 2147483647. || r < -2147483648.)
      png_fixed_error(png_ptr, text);

#  ifndef PNG_ERROR_TEXT_SUPPORTED
   PNG_UNUSED(text)
#  endif

   return (png_fixed_point)r;
}