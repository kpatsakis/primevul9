png_XYZ_normalize(png_XYZ *XYZ)
{
   png_int_32 Y;

   if (XYZ->red_Y < 0 || XYZ->green_Y < 0 || XYZ->blue_Y < 0 ||
      XYZ->red_X < 0 || XYZ->green_X < 0 || XYZ->blue_X < 0 ||
      XYZ->red_Z < 0 || XYZ->green_Z < 0 || XYZ->blue_Z < 0)
      return 1;

   /* Normalize by scaling so the sum of the end-point Y values is PNG_FP_1.
    * IMPLEMENTATION NOTE: ANSI requires signed overflow not to occur, therefore
    * relying on addition of two positive values producing a negative one is not
    * safe.
    */
   Y = XYZ->red_Y;
   if (0x7fffffff - Y < XYZ->green_X)
      return 1;
   Y += XYZ->green_Y;
   if (0x7fffffff - Y < XYZ->blue_X)
      return 1;
   Y += XYZ->blue_Y;

   if (Y != PNG_FP_1)
   {
      if (png_muldiv(&XYZ->red_X, XYZ->red_X, PNG_FP_1, Y) == 0)
         return 1;
      if (png_muldiv(&XYZ->red_Y, XYZ->red_Y, PNG_FP_1, Y) == 0)
         return 1;
      if (png_muldiv(&XYZ->red_Z, XYZ->red_Z, PNG_FP_1, Y) == 0)
         return 1;

      if (png_muldiv(&XYZ->green_X, XYZ->green_X, PNG_FP_1, Y) == 0)
         return 1;
      if (png_muldiv(&XYZ->green_Y, XYZ->green_Y, PNG_FP_1, Y) == 0)
         return 1;
      if (png_muldiv(&XYZ->green_Z, XYZ->green_Z, PNG_FP_1, Y) == 0)
         return 1;

      if (png_muldiv(&XYZ->blue_X, XYZ->blue_X, PNG_FP_1, Y) == 0)
         return 1;
      if (png_muldiv(&XYZ->blue_Y, XYZ->blue_Y, PNG_FP_1, Y) == 0)
         return 1;
      if (png_muldiv(&XYZ->blue_Z, XYZ->blue_Z, PNG_FP_1, Y) == 0)
         return 1;
   }

   return 0;
}