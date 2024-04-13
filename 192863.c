png_build_16to8_table(png_structrp png_ptr, png_uint_16pp *ptable,
    PNG_CONST unsigned int shift, PNG_CONST png_fixed_point gamma_val)
{
   PNG_CONST unsigned int num = 1U << (8U - shift);
   PNG_CONST unsigned int max = (1U << (16U - shift))-1U;
   unsigned int i;
   png_uint_32 last;

   png_uint_16pp table = *ptable =
       (png_uint_16pp)png_calloc(png_ptr, num * (sizeof (png_uint_16p)));

   /* 'num' is the number of tables and also the number of low bits of low
    * bits of the input 16-bit value used to select a table.  Each table is
    * itself indexed by the high 8 bits of the value.
    */
   for (i = 0; i < num; i++)
      table[i] = (png_uint_16p)png_malloc(png_ptr,
          256 * (sizeof (png_uint_16)));

   /* 'gamma_val' is set to the reciprocal of the value calculated above, so
    * pow(out,g) is an *input* value.  'last' is the last input value set.
    *
    * In the loop 'i' is used to find output values.  Since the output is
    * 8-bit there are only 256 possible values.  The tables are set up to
    * select the closest possible output value for each input by finding
    * the input value at the boundary between each pair of output values
    * and filling the table up to that boundary with the lower output
    * value.
    *
    * The boundary values are 0.5,1.5..253.5,254.5.  Since these are 9-bit
    * values the code below uses a 16-bit value in i; the values start at
    * 128.5 (for 0.5) and step by 257, for a total of 254 values (the last
    * entries are filled with 255).  Start i at 128 and fill all 'last'
    * table entries <= 'max'
    */
   last = 0;
   for (i = 0; i < 255; ++i) /* 8-bit output value */
   {
      /* Find the corresponding maximum input value */
      png_uint_16 out = (png_uint_16)(i * 257U); /* 16-bit output value */

      /* Find the boundary value in 16 bits: */
      png_uint_32 bound = png_gamma_16bit_correct(out+128U, gamma_val);

      /* Adjust (round) to (16-shift) bits: */
      bound = (bound * max + 32768U)/65535U + 1U;

      while (last < bound)
      {
         table[last & (0xffU >> shift)][last >> (8U - shift)] = out;
         last++;
      }
   }

   /* And fill in the final entries. */
   while (last < (num << 8))
   {
      table[last & (0xff >> shift)][last >> (8U - shift)] = 65535U;
      last++;
   }
}