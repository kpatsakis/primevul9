png_colorspace_check_XYZ(png_xy *xy, png_XYZ *XYZ)
{
   int result;
   png_XYZ XYZtemp;

   result = png_XYZ_normalize(XYZ);
   if (result != 0)
      return result;

   result = png_xy_from_XYZ(xy, XYZ);
   if (result != 0)
      return result;

   XYZtemp = *XYZ;
   return png_colorspace_check_xy(&XYZtemp, xy);
}