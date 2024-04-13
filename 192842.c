png_build_16bit_table(png_structrp png_ptr, png_uint_16pp *ptable,
    PNG_CONST unsigned int shift, PNG_CONST png_fixed_point gamma_val)
{
   /* Various values derived from 'shift': */
   PNG_CONST unsigned int num = 1U << (8U - shift);
#ifdef PNG_FLOATING_ARITHMETIC_SUPPORTED
   /* CSE the division and work round wacky GCC warnings (see the comments
    * in png_gamma_8bit_correct for where these come from.)
    */
   PNG_CONST double fmax = 1./(((png_int_32)1 << (16U - shift))-1);
#endif
   PNG_CONST unsigned int max = (1U << (16U - shift))-1U;
   PNG_CONST unsigned int max_by_2 = 1U << (15U-shift);
   unsigned int i;

   png_uint_16pp table = *ptable =
       (png_uint_16pp)png_calloc(png_ptr, num * (sizeof (png_uint_16p)));

   for (i = 0; i < num; i++)
   {
      png_uint_16p sub_table = table[i] =
          (png_uint_16p)png_malloc(png_ptr, 256 * (sizeof (png_uint_16)));

      /* The 'threshold' test is repeated here because it can arise for one of
       * the 16-bit tables even if the others don't hit it.
       */
      if (png_gamma_significant(gamma_val) != 0)
      {
         /* The old code would overflow at the end and this would cause the
          * 'pow' function to return a result >1, resulting in an
          * arithmetic error.  This code follows the spec exactly; ig is
          * the recovered input sample, it always has 8-16 bits.
          *
          * We want input * 65535/max, rounded, the arithmetic fits in 32
          * bits (unsigned) so long as max <= 32767.
          */
         unsigned int j;
         for (j = 0; j < 256; j++)
         {
            png_uint_32 ig = (j << (8-shift)) + i;
#           ifdef PNG_FLOATING_ARITHMETIC_SUPPORTED
               /* Inline the 'max' scaling operation: */
               /* See png_gamma_8bit_correct for why the cast to (int) is
                * required here.
                */
               double d = floor(65535.*pow(ig*fmax, gamma_val*.00001)+.5);
               sub_table[j] = (png_uint_16)d;
#           else
               if (shift != 0)
                  ig = (ig * 65535U + max_by_2)/max;

               sub_table[j] = png_gamma_16bit_correct(ig, gamma_val);
#           endif
         }
      }
      else
      {
         /* We must still build a table, but do it the fast way. */
         unsigned int j;

         for (j = 0; j < 256; j++)
         {
            png_uint_32 ig = (j << (8-shift)) + i;

            if (shift != 0)
               ig = (ig * 65535U + max_by_2)/max;

            sub_table[j] = (png_uint_16)ig;
         }
      }
   }
}