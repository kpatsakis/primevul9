png_log16bit(png_uint_32 x)
{
   unsigned int lg2 = 0;

   /* As above, but now the input has 16 bits. */
   if ((x &= 0xffff) == 0)
      return -1;

   if ((x & 0xff00) == 0)
      lg2  = 8, x <<= 8;

   if ((x & 0xf000) == 0)
      lg2 += 4, x <<= 4;

   if ((x & 0xc000) == 0)
      lg2 += 2, x <<= 2;

   if ((x & 0x8000) == 0)
      lg2 += 1, x <<= 1;

   /* Calculate the base logarithm from the top 8 bits as a 28-bit fractional
    * value.
    */
   lg2 <<= 28;
   lg2 += (png_8bit_l2[(x>>8)-128]+8) >> 4;

   /* Now we need to interpolate the factor, this requires a division by the top
    * 8 bits.  Do this with maximum precision.
    */
   x = ((x << 16) + (x >> 9)) / (x >> 8);

   /* Since we divided by the top 8 bits of 'x' there will be a '1' at 1<<24,
    * the value at 1<<16 (ignoring this) will be 0 or 1; this gives us exactly
    * 16 bits to interpolate to get the low bits of the result.  Round the
    * answer.  Note that the end point values are scaled by 64 to retain overall
    * precision and that 'lg2' is current scaled by an extra 12 bits, so adjust
    * the overall scaling by 6-12.  Round at every step.
    */
   x -= 1U << 24;

   if (x <= 65536U) /* <= '257' */
      lg2 += ((23591U * (65536U-x)) + (1U << (16+6-12-1))) >> (16+6-12);

   else
      lg2 -= ((23499U * (x-65536U)) + (1U << (16+6-12-1))) >> (16+6-12);

   /* Safe, because the result can't have more than 20 bits: */
   return (png_int_32)((lg2 + 2048) >> 12);
}