png_destroy_info_struct(png_const_structrp png_ptr, png_infopp info_ptr_ptr)
{
   png_inforp info_ptr = NULL;

   png_debug(1, "in png_destroy_info_struct");

   if (png_ptr == NULL)
      return;

   if (info_ptr_ptr != NULL)
      info_ptr = *info_ptr_ptr;

   if (info_ptr != NULL)
   {
      /* Do this first in case of an error below; if the app implements its own
       * memory management this can lead to png_free calling png_error, which
       * will abort this routine and return control to the app error handler.
       * An infinite loop may result if it then tries to free the same info
       * ptr.
       */
      *info_ptr_ptr = NULL;

      png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
      memset(info_ptr, 0, (sizeof *info_ptr));
      png_free(png_ptr, info_ptr);
   }
}