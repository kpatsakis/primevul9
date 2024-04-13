png_reciprocal2(png_fixed_point a, png_fixed_point b)
{
   /* The required result is 1/a * 1/b; the following preserves accuracy. */
#ifdef PNG_FLOATING_ARITHMETIC_SUPPORTED
   if (a != 0 && b != 0)
   {
      double r = 1E15/a;
      r /= b;
      r = floor(r+.5);

      if (r <= 2147483647. && r >= -2147483648.)
         return (png_fixed_point)r;
   }
#else
   /* This may overflow because the range of png_fixed_point isn't symmetric,
    * but this API is only used for the product of file and screen gamma so it
    * doesn't matter that the smallest number it can produce is 1/21474, not
    * 1/100000
    */
   png_fixed_point res = png_product2(a, b);

   if (res != 0)
      return png_reciprocal(res);
#endif

   return 0; /* overflow */
}