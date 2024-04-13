png_colorspace_set_endpoints(png_const_structrp png_ptr,
    png_colorspacerp colorspace, const png_XYZ *XYZ_in, int preferred)
{
   png_XYZ XYZ = *XYZ_in;
   png_xy xy;

   switch (png_colorspace_check_XYZ(&xy, &XYZ))
   {
      case 0:
         return png_colorspace_set_xy_and_XYZ(png_ptr, colorspace, &xy, &XYZ,
             preferred);

      case 1:
         /* End points are invalid. */
         colorspace->flags |= PNG_COLORSPACE_INVALID;
         png_benign_error(png_ptr, "invalid end points");
         break;

      default:
         colorspace->flags |= PNG_COLORSPACE_INVALID;
         png_error(png_ptr, "internal error checking chromaticities");
   }

   return 0; /* failed */
}