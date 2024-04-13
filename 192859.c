png_handle_as_unknown(png_const_structrp png_ptr, png_const_bytep chunk_name)
{
   /* Check chunk_name and return "keep" value if it's on the list, else 0 */
   png_const_bytep p, p_end;

   if (png_ptr == NULL || chunk_name == NULL || png_ptr->num_chunk_list == 0)
      return PNG_HANDLE_CHUNK_AS_DEFAULT;

   p_end = png_ptr->chunk_list;
   p = p_end + png_ptr->num_chunk_list*5; /* beyond end */

   /* The code is the fifth byte after each four byte string.  Historically this
    * code was always searched from the end of the list, this is no longer
    * necessary because the 'set' routine handles duplicate entries correcty.
    */
   do /* num_chunk_list > 0, so at least one */
   {
      p -= 5;

      if (memcmp(chunk_name, p, 4) == 0)
         return p[4];
   }
   while (p > p_end);

   /* This means that known chunks should be processed and unknown chunks should
    * be handled according to the value of png_ptr->unknown_default; this can be
    * confusing because, as a result, there are two levels of defaulting for
    * unknown chunks.
    */
   return PNG_HANDLE_CHUNK_AS_DEFAULT;
}