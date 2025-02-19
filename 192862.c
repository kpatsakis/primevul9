png_colorspace_set_sRGB(png_const_structrp png_ptr, png_colorspacerp colorspace,
    int intent)
{
   /* sRGB sets known gamma, end points and (from the chunk) intent. */
   /* IMPORTANT: these are not necessarily the values found in an ICC profile
    * because ICC profiles store values adapted to a D50 environment; it is
    * expected that the ICC profile mediaWhitePointTag will be D50; see the
    * checks and code elsewhere to understand this better.
    *
    * These XYZ values, which are accurate to 5dp, produce rgb to gray
    * coefficients of (6968,23435,2366), which are reduced (because they add up
    * to 32769 not 32768) to (6968,23434,2366).  These are the values that
    * libpng has traditionally used (and are the best values given the 15bit
    * algorithm used by the rgb to gray code.)
    */
   static const png_XYZ sRGB_XYZ = /* D65 XYZ (*not* the D50 adapted values!) */
   {
      /* color      X      Y      Z */
      /* red   */ 41239, 21264,  1933,
      /* green */ 35758, 71517, 11919,
      /* blue  */ 18048,  7219, 95053
   };

   /* Do nothing if the colorspace is already invalidated. */
   if ((colorspace->flags & PNG_COLORSPACE_INVALID) != 0)
      return 0;

   /* Check the intent, then check for existing settings.  It is valid for the
    * PNG file to have cHRM or gAMA chunks along with sRGB, but the values must
    * be consistent with the correct values.  If, however, this function is
    * called below because an iCCP chunk matches sRGB then it is quite
    * conceivable that an older app recorded incorrect gAMA and cHRM because of
    * an incorrect calculation based on the values in the profile - this does
    * *not* invalidate the profile (though it still produces an error, which can
    * be ignored.)
    */
   if (intent < 0 || intent >= PNG_sRGB_INTENT_LAST)
      return png_icc_profile_error(png_ptr, colorspace, "sRGB",
          (unsigned)intent, "invalid sRGB rendering intent");

   if ((colorspace->flags & PNG_COLORSPACE_HAVE_INTENT) != 0 &&
       colorspace->rendering_intent != intent)
      return png_icc_profile_error(png_ptr, colorspace, "sRGB",
         (unsigned)intent, "inconsistent rendering intents");

   if ((colorspace->flags & PNG_COLORSPACE_FROM_sRGB) != 0)
   {
      png_benign_error(png_ptr, "duplicate sRGB information ignored");
      return 0;
   }

   /* If the standard sRGB cHRM chunk does not match the one from the PNG file
    * warn but overwrite the value with the correct one.
    */
   if ((colorspace->flags & PNG_COLORSPACE_HAVE_ENDPOINTS) != 0 &&
       !png_colorspace_endpoints_match(&sRGB_xy, &colorspace->end_points_xy,
       100))
      png_chunk_report(png_ptr, "cHRM chunk does not match sRGB",
         PNG_CHUNK_ERROR);

   /* This check is just done for the error reporting - the routine always
    * returns true when the 'from' argument corresponds to sRGB (2).
    */
   (void)png_colorspace_check_gamma(png_ptr, colorspace, PNG_GAMMA_sRGB_INVERSE,
       2/*from sRGB*/);

   /* intent: bugs in GCC force 'int' to be used as the parameter type. */
   colorspace->rendering_intent = (png_uint_16)intent;
   colorspace->flags |= PNG_COLORSPACE_HAVE_INTENT;

   /* endpoints */
   colorspace->end_points_xy = sRGB_xy;
   colorspace->end_points_XYZ = sRGB_XYZ;
   colorspace->flags |=
      (PNG_COLORSPACE_HAVE_ENDPOINTS|PNG_COLORSPACE_ENDPOINTS_MATCH_sRGB);

   /* gamma */
   colorspace->gamma = PNG_GAMMA_sRGB_INVERSE;
   colorspace->flags |= PNG_COLORSPACE_HAVE_GAMMA;

   /* Finally record that we have an sRGB profile */
   colorspace->flags |=
      (PNG_COLORSPACE_MATCHES_sRGB|PNG_COLORSPACE_FROM_sRGB);

   return 1; /* set */
}