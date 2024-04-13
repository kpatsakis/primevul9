png_colorspace_sync_info(png_const_structrp png_ptr, png_inforp info_ptr)
{
   if ((info_ptr->colorspace.flags & PNG_COLORSPACE_INVALID) != 0)
   {
      /* Everything is invalid */
      info_ptr->valid &= ~(PNG_INFO_gAMA|PNG_INFO_cHRM|PNG_INFO_sRGB|
         PNG_INFO_iCCP);

#     ifdef PNG_COLORSPACE_SUPPORTED
      /* Clean up the iCCP profile now if it won't be used. */
      png_free_data(png_ptr, info_ptr, PNG_FREE_ICCP, -1/*not used*/);
#     else
      PNG_UNUSED(png_ptr)
#     endif
   }

   else
   {
#     ifdef PNG_COLORSPACE_SUPPORTED
      /* Leave the INFO_iCCP flag set if the pngset.c code has already set
       * it; this allows a PNG to contain a profile which matches sRGB and
       * yet still have that profile retrievable by the application.
       */
      if ((info_ptr->colorspace.flags & PNG_COLORSPACE_MATCHES_sRGB) != 0)
         info_ptr->valid |= PNG_INFO_sRGB;

      else
         info_ptr->valid &= ~PNG_INFO_sRGB;

      if ((info_ptr->colorspace.flags & PNG_COLORSPACE_HAVE_ENDPOINTS) != 0)
         info_ptr->valid |= PNG_INFO_cHRM;

      else
         info_ptr->valid &= ~PNG_INFO_cHRM;
#     endif

      if ((info_ptr->colorspace.flags & PNG_COLORSPACE_HAVE_GAMMA) != 0)
         info_ptr->valid |= PNG_INFO_gAMA;

      else
         info_ptr->valid &= ~PNG_INFO_gAMA;
   }
}