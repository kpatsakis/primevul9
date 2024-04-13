png_muldiv_warn(png_const_structrp png_ptr, png_fixed_point a, png_int_32 times,
    png_int_32 divisor)
{
   png_fixed_point result;

   if (png_muldiv(&result, a, times, divisor) != 0)
      return result;

   png_warning(png_ptr, "fixed point overflow ignored");
   return 0;
}