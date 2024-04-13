png_set_option(png_structrp png_ptr, int option, int onoff)
{
   if (png_ptr != NULL && option >= 0 && option < PNG_OPTION_NEXT &&
      (option & 1) == 0)
   {
      int mask = 3 << option;
      int setting = (2 + (onoff != 0)) << option;
      int current = png_ptr->options;

      png_ptr->options = (png_byte)(((current & ~mask) | setting) & 0xff);

      return (current & mask) >> option;
   }

   return PNG_OPTION_INVALID;
}