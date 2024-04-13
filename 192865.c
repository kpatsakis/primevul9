png_colorspace_set_chromaticities(png_const_structrp png_ptr,
    png_colorspacerp colorspace, const png_xy *xy, int preferred)
{
   /* We must check the end points to ensure they are reasonable - in the past
    * color management systems have crashed as a result of getting bogus
    * colorant values, while this isn't the fault of libpng it is the
    * responsibility of libpng because PNG carries the bomb and libpng is in a
    * position to protect against it.
    */
   png_XYZ XYZ;

   switch (png_colorspace_check_xy(&XYZ, xy))
   {
      case 0: /* success */
         return png_colorspace_set_xy_and_XYZ(png_ptr, colorspace, xy, &XYZ,
             preferred);

      case 1:
         /* We can't invert the chromaticities so we can't produce value XYZ
          * values.  Likely as not a color management system will fail too.
          */
         colorspace->flags |= PNG_COLORSPACE_INVALID;
         png_benign_error(png_ptr, "invalid chromaticities");
         break;

      default:
         /* libpng is broken; this should be a warning but if it happens we
          * want error reports so for the moment it is an error.
          */
         colorspace->flags |= PNG_COLORSPACE_INVALID;
         png_error(png_ptr, "internal error checking chromaticities");
   }

   return 0; /* failed */
}