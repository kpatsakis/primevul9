png_exp16bit(png_fixed_point lg2)
{
   /* Get a 32-bit value: */
   png_uint_32 x = png_exp(lg2);

   /* Convert the 32-bit value to 0..65535 by multiplying by 65536-1: */
   x -= x >> 16;
   return (png_uint_16)((x + 32767U) >> 16);
}