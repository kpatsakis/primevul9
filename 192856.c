PNG_FUNCTION(png_infop,PNGAPI
png_create_info_struct,(png_const_structrp png_ptr),PNG_ALLOCATED)
{
   png_inforp info_ptr;

   png_debug(1, "in png_create_info_struct");

   if (png_ptr == NULL)
      return NULL;

   /* Use the internal API that does not (or at least should not) error out, so
    * that this call always returns ok.  The application typically sets up the
    * error handling *after* creating the info_struct because this is the way it
    * has always been done in 'example.c'.
    */
   info_ptr = png_voidcast(png_inforp, png_malloc_base(png_ptr,
       (sizeof *info_ptr)));

   if (info_ptr != NULL)
      memset(info_ptr, 0, (sizeof *info_ptr));

   return info_ptr;
}