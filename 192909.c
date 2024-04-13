png_colorspace_check_xy(png_XYZ *XYZ, const png_xy *xy)
{
   int result;
   png_xy xy_test;

   /* As a side-effect this routine also returns the XYZ endpoints. */
   result = png_XYZ_from_xy(XYZ, xy);
   if (result != 0)
      return result;

   result = png_xy_from_XYZ(&xy_test, XYZ);
   if (result != 0)
      return result;

   if (png_colorspace_endpoints_match(xy, &xy_test,
       5/*actually, the math is pretty accurate*/) != 0)
      return 0;

   /* Too much slip */
   return 1;
}