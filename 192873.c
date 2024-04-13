png_reset_zstream(png_structrp png_ptr)
{
   if (png_ptr == NULL)
      return Z_STREAM_ERROR;

   /* WARNING: this resets the window bits to the maximum! */
   return (inflateReset(&png_ptr->zstream));
}