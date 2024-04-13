png_icc_profile_error(png_const_structrp png_ptr, png_colorspacerp colorspace,
    png_const_charp name, png_alloc_size_t value, png_const_charp reason)
{
   size_t pos;
   char message[196]; /* see below for calculation */

   if (colorspace != NULL)
      colorspace->flags |= PNG_COLORSPACE_INVALID;

   pos = png_safecat(message, (sizeof message), 0, "profile '"); /* 9 chars */
   pos = png_safecat(message, pos+79, pos, name); /* Truncate to 79 chars */
   pos = png_safecat(message, (sizeof message), pos, "': "); /* +2 = 90 */
   if (is_ICC_signature(value) != 0)
   {
      /* So 'value' is at most 4 bytes and the following cast is safe */
      png_icc_tag_name(message+pos, (png_uint_32)value);
      pos += 6; /* total +8; less than the else clause */
      message[pos++] = ':';
      message[pos++] = ' ';
   }
#  ifdef PNG_WARNINGS_SUPPORTED
   else
      {
         char number[PNG_NUMBER_BUFFER_SIZE]; /* +24 = 114*/

         pos = png_safecat(message, (sizeof message), pos,
             png_format_number(number, number+(sizeof number),
             PNG_NUMBER_FORMAT_x, value));
         pos = png_safecat(message, (sizeof message), pos, "h: "); /*+2 = 116*/
      }
#  endif
   /* The 'reason' is an arbitrary message, allow +79 maximum 195 */
   pos = png_safecat(message, (sizeof message), pos, reason);
   PNG_UNUSED(pos)

   /* This is recoverable, but make it unconditionally an app_error on write to
    * avoid writing invalid ICC profiles into PNG files (i.e., we handle them
    * on read, with a warning, but on write unless the app turns off
    * application errors the PNG won't be written.)
    */
   png_chunk_report(png_ptr, message,
       (colorspace != NULL) ? PNG_CHUNK_ERROR : PNG_CHUNK_WRITE_ERROR);

   return 0;
}