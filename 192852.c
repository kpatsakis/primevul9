png_ascii_from_fixed(png_const_structrp png_ptr, png_charp ascii,
    png_size_t size, png_fixed_point fp)
{
   /* Require space for 10 decimal digits, a decimal point, a minus sign and a
    * trailing \0, 13 characters:
    */
   if (size > 12)
   {
      png_uint_32 num;

      /* Avoid overflow here on the minimum integer. */
      if (fp < 0)
         *ascii++ = 45, num = (png_uint_32)(-fp);
      else
         num = (png_uint_32)fp;

      if (num <= 0x80000000) /* else overflowed */
      {
         unsigned int ndigits = 0, first = 16 /* flag value */;
         char digits[10];

         while (num)
         {
            /* Split the low digit off num: */
            unsigned int tmp = num/10;
            num -= tmp*10;
            digits[ndigits++] = (char)(48 + num);
            /* Record the first non-zero digit, note that this is a number
             * starting at 1, it's not actually the array index.
             */
            if (first == 16 && num > 0)
               first = ndigits;
            num = tmp;
         }

         if (ndigits > 0)
         {
            while (ndigits > 5) *ascii++ = digits[--ndigits];
            /* The remaining digits are fractional digits, ndigits is '5' or
             * smaller at this point.  It is certainly not zero.  Check for a
             * non-zero fractional digit:
             */
            if (first <= 5)
            {
               unsigned int i;
               *ascii++ = 46; /* decimal point */
               /* ndigits may be <5 for small numbers, output leading zeros
                * then ndigits digits to first:
                */
               i = 5;
               while (ndigits < i) *ascii++ = 48, --i;
               while (ndigits >= first) *ascii++ = digits[--ndigits];
               /* Don't output the trailing zeros! */
            }
         }
         else
            *ascii++ = 48;

         /* And null terminate the string: */
         *ascii = 0;
         return;
      }
   }

   /* Here on buffer too small. */
   png_error(png_ptr, "ASCII conversion buffer too small");
}