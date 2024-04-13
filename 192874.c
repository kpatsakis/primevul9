png_colorspace_set_xy_and_XYZ(png_const_structrp png_ptr,
    png_colorspacerp colorspace, const png_xy *xy, const png_XYZ *XYZ,
    int preferred)
{
   if ((colorspace->flags & PNG_COLORSPACE_INVALID) != 0)
      return 0;

   /* The consistency check is performed on the chromaticities; this factors out
    * variations because of the normalization (or not) of the end point Y
    * values.
    */
   if (preferred < 2 &&
       (colorspace->flags & PNG_COLORSPACE_HAVE_ENDPOINTS) != 0)
   {
      /* The end points must be reasonably close to any we already have.  The
       * following allows an error of up to +/-.001
       */
      if (png_colorspace_endpoints_match(xy, &colorspace->end_points_xy,
          100) == 0)
      {
         colorspace->flags |= PNG_COLORSPACE_INVALID;
         png_benign_error(png_ptr, "inconsistent chromaticities");
         return 0; /* failed */
      }

      /* Only overwrite with preferred values */
      if (preferred == 0)
         return 1; /* ok, but no change */
   }

   colorspace->end_points_xy = *xy;
   colorspace->end_points_XYZ = *XYZ;
   colorspace->flags |= PNG_COLORSPACE_HAVE_ENDPOINTS;

   /* The end points are normally quoted to two decimal digits, so allow +/-0.01
    * on this test.
    */
   if (png_colorspace_endpoints_match(xy, &sRGB_xy, 1000) != 0)
      colorspace->flags |= PNG_COLORSPACE_ENDPOINTS_MATCH_sRGB;

   else
      colorspace->flags &= PNG_COLORSPACE_CANCEL(
         PNG_COLORSPACE_ENDPOINTS_MATCH_sRGB);

   return 2; /* ok and changed */
}