png_colorspace_sync(png_const_structrp png_ptr, png_inforp info_ptr)
{
   if (info_ptr == NULL) /* reduce code size; check here not in the caller */
      return;

   info_ptr->colorspace = png_ptr->colorspace;
   png_colorspace_sync_info(png_ptr, info_ptr);
}