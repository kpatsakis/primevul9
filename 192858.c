png_gamma_16bit_correct(unsigned int value, png_fixed_point gamma_val)
{
   if (value > 0 && value < 65535)
   {
# ifdef PNG_FLOATING_ARITHMETIC_SUPPORTED
      /* The same (unsigned int)->(double) constraints apply here as above,
       * however in this case the (unsigned int) to (int) conversion can
       * overflow on an ANSI-C90 compliant system so the cast needs to ensure
       * that this is not possible.
       */
      double r = floor(65535*pow((png_int_32)value/65535.,
          gamma_val*.00001)+.5);
      return (png_uint_16)r;
# else
      png_int_32 lg2 = png_log16bit(value);
      png_fixed_point res;

      if (png_muldiv(&res, gamma_val, lg2, PNG_FP_1) != 0)
         return png_exp16bit(res);

      /* Overflow. */
      value = 0;
# endif
   }

   return (png_uint_16)value;
}