png_convert_to_rfc1123(png_structrp png_ptr, png_const_timep ptime)
{
   if (png_ptr != NULL)
   {
      /* The only failure above if png_ptr != NULL is from an invalid ptime */
      if (png_convert_to_rfc1123_buffer(png_ptr->time_buffer, ptime) == 0)
         png_warning(png_ptr, "Ignoring invalid time value");

      else
         return png_ptr->time_buffer;
   }

   return NULL;
}