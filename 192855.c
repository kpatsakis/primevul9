png_reciprocal(png_fixed_point a)
{
#ifdef PNG_FLOATING_ARITHMETIC_SUPPORTED
   double r = floor(1E10/a+.5);

   if (r <= 2147483647. && r >= -2147483648.)
      return (png_fixed_point)r;
#else
   png_fixed_point res;

   if (png_muldiv(&res, 100000, 100000, a) != 0)
      return res;
#endif

   return 0; /* error/overflow */
}