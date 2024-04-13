png_xy_from_XYZ(png_xy *xy, const png_XYZ *XYZ)
{
   png_int_32 d, dwhite, whiteX, whiteY;

   d = XYZ->red_X + XYZ->red_Y + XYZ->red_Z;
   if (png_muldiv(&xy->redx, XYZ->red_X, PNG_FP_1, d) == 0)
      return 1;
   if (png_muldiv(&xy->redy, XYZ->red_Y, PNG_FP_1, d) == 0)
      return 1;
   dwhite = d;
   whiteX = XYZ->red_X;
   whiteY = XYZ->red_Y;

   d = XYZ->green_X + XYZ->green_Y + XYZ->green_Z;
   if (png_muldiv(&xy->greenx, XYZ->green_X, PNG_FP_1, d) == 0)
      return 1;
   if (png_muldiv(&xy->greeny, XYZ->green_Y, PNG_FP_1, d) == 0)
      return 1;
   dwhite += d;
   whiteX += XYZ->green_X;
   whiteY += XYZ->green_Y;

   d = XYZ->blue_X + XYZ->blue_Y + XYZ->blue_Z;
   if (png_muldiv(&xy->bluex, XYZ->blue_X, PNG_FP_1, d) == 0)
      return 1;
   if (png_muldiv(&xy->bluey, XYZ->blue_Y, PNG_FP_1, d) == 0)
      return 1;
   dwhite += d;
   whiteX += XYZ->blue_X;
   whiteY += XYZ->blue_Y;

   /* The reference white is simply the sum of the end-point (X,Y,Z) vectors,
    * thus:
    */
   if (png_muldiv(&xy->whitex, whiteX, PNG_FP_1, dwhite) == 0)
      return 1;
   if (png_muldiv(&xy->whitey, whiteY, PNG_FP_1, dwhite) == 0)
      return 1;

   return 0;
}