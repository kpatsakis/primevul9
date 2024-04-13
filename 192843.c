png_set_sig_bytes(png_structrp png_ptr, int num_bytes)
{
   unsigned int nb = (unsigned int)num_bytes;

   png_debug(1, "in png_set_sig_bytes");

   if (png_ptr == NULL)
      return;

   if (num_bytes < 0)
      nb = 0;

   if (nb > 8)
      png_error(png_ptr, "Too many bytes for PNG signature");

   png_ptr->sig_bytes = (png_byte)nb;
}