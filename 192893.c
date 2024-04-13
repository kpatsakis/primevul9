png_log8bit(unsigned int x)
{
   unsigned int lg2 = 0;
   /* Each time 'x' is multiplied by 2, 1 must be subtracted off the final log,
    * because the log is actually negate that means adding 1.  The final
    * returned value thus has the range 0 (for 255 input) to 7.994 (for 1
    * input), return -1 for the overflow (log 0) case, - so the result is
    * always at most 19 bits.
    */
   if ((x &= 0xff) == 0)
      return -1;

   if ((x & 0xf0) == 0)
      lg2  = 4, x <<= 4;

   if ((x & 0xc0) == 0)
      lg2 += 2, x <<= 2;

   if ((x & 0x80) == 0)
      lg2 += 1, x <<= 1;

   /* result is at most 19 bits, so this cast is safe: */
   return (png_int_32)((lg2 << 16) + ((png_8bit_l2[x-128]+32768)>>16));
}