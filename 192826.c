png_exp8bit(png_fixed_point lg2)
{
   /* Get a 32-bit value: */
   png_uint_32 x = png_exp(lg2);

   /* Convert the 32-bit value to 0..255 by multiplying by 256-1. Note that the
    * second, rounding, step can't overflow because of the first, subtraction,
    * step.
    */
   x -= x >> 8;
   return (png_byte)(((x + 0x7fffffU) >> 24) & 0xff);
}