png_chunk_unknown_handling(png_const_structrp png_ptr, png_uint_32 chunk_name)
{
   png_byte chunk_string[5];

   PNG_CSTRING_FROM_CHUNK(chunk_string, chunk_name);
   return png_handle_as_unknown(png_ptr, chunk_string);
}