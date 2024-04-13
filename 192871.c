PNG_FUNCTION(void,PNGAPI
png_info_init_3,(png_infopp ptr_ptr, png_size_t png_info_struct_size),
    PNG_DEPRECATED)
{
   png_inforp info_ptr = *ptr_ptr;

   png_debug(1, "in png_info_init_3");

   if (info_ptr == NULL)
      return;

   if ((sizeof (png_info)) > png_info_struct_size)
   {
      *ptr_ptr = NULL;
      /* The following line is why this API should not be used: */
      free(info_ptr);
      info_ptr = png_voidcast(png_inforp, png_malloc_base(NULL,
          (sizeof *info_ptr)));
      if (info_ptr == NULL)
         return;
      *ptr_ptr = info_ptr;
   }

   /* Set everything to 0 */
   memset(info_ptr, 0, (sizeof *info_ptr));
}