png_reset_crc(png_structrp png_ptr)
{
   /* The cast is safe because the crc is a 32-bit value. */
   png_ptr->crc = (png_uint_32)crc32(0, Z_NULL, 0);
}