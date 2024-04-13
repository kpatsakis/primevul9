png_colorspace_check_gamma(png_const_structrp png_ptr,
    png_colorspacerp colorspace, png_fixed_point gAMA, int from)
   /* This is called to check a new gamma value against an existing one.  The
    * routine returns false if the new gamma value should not be written.
    *
    * 'from' says where the new gamma value comes from:
    *
    *    0: the new gamma value is the libpng estimate for an ICC profile
    *    1: the new gamma value comes from a gAMA chunk
    *    2: the new gamma value comes from an sRGB chunk
    */
{
   png_fixed_point gtest;

   if ((colorspace->flags & PNG_COLORSPACE_HAVE_GAMMA) != 0 &&
       (png_muldiv(&gtest, colorspace->gamma, PNG_FP_1, gAMA) == 0  ||
      png_gamma_significant(gtest) != 0))
   {
      /* Either this is an sRGB image, in which case the calculated gamma
       * approximation should match, or this is an image with a profile and the
       * value libpng calculates for the gamma of the profile does not match the
       * value recorded in the file.  The former, sRGB, case is an error, the
       * latter is just a warning.
       */
      if ((colorspace->flags & PNG_COLORSPACE_FROM_sRGB) != 0 || from == 2)
      {
         png_chunk_report(png_ptr, "gamma value does not match sRGB",
             PNG_CHUNK_ERROR);
         /* Do not overwrite an sRGB value */
         return from == 2;
      }

      else /* sRGB tag not involved */
      {
         png_chunk_report(png_ptr, "gamma value does not match libpng estimate",
             PNG_CHUNK_WARNING);
         return from == 1;
      }
   }

   return 1;
}