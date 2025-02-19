png_colorspace_set_rgb_coefficients(png_structrp png_ptr)
{
   /* Set the rgb_to_gray coefficients from the colorspace. */
   if (png_ptr->rgb_to_gray_coefficients_set == 0 &&
      (png_ptr->colorspace.flags & PNG_COLORSPACE_HAVE_ENDPOINTS) != 0)
   {
      /* png_set_background has not been called, get the coefficients from the Y
       * values of the colorspace colorants.
       */
      png_fixed_point r = png_ptr->colorspace.end_points_XYZ.red_Y;
      png_fixed_point g = png_ptr->colorspace.end_points_XYZ.green_Y;
      png_fixed_point b = png_ptr->colorspace.end_points_XYZ.blue_Y;
      png_fixed_point total = r+g+b;

      if (total > 0 &&
         r >= 0 && png_muldiv(&r, r, 32768, total) && r >= 0 && r <= 32768 &&
         g >= 0 && png_muldiv(&g, g, 32768, total) && g >= 0 && g <= 32768 &&
         b >= 0 && png_muldiv(&b, b, 32768, total) && b >= 0 && b <= 32768 &&
         r+g+b <= 32769)
      {
         /* We allow 0 coefficients here.  r+g+b may be 32769 if two or
          * all of the coefficients were rounded up.  Handle this by
          * reducing the *largest* coefficient by 1; this matches the
          * approach used for the default coefficients in pngrtran.c
          */
         int add = 0;

         if (r+g+b > 32768)
            add = -1;
         else if (r+g+b < 32768)
            add = 1;

         if (add != 0)
         {
            if (g >= r && g >= b)
               g += add;
            else if (r >= g && r >= b)
               r += add;
            else
               b += add;
         }

         /* Check for an internal error. */
         if (r+g+b != 32768)
            png_error(png_ptr,
                "internal error handling cHRM coefficients");

         else
         {
            png_ptr->rgb_to_gray_red_coeff   = (png_uint_16)r;
            png_ptr->rgb_to_gray_green_coeff = (png_uint_16)g;
         }
      }

      /* This is a png_error at present even though it could be ignored -
       * it should never happen, but it is important that if it does, the
       * bug is fixed.
       */
      else
         png_error(png_ptr, "internal error handling cHRM->XYZ");
   }
}