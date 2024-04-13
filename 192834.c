png_convert_to_rfc1123_buffer(char out[29], png_const_timep ptime)
{
   static PNG_CONST char short_months[12][4] =
        {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
         "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

   if (out == NULL)
      return 0;

   if (ptime->year > 9999 /* RFC1123 limitation */ ||
       ptime->month == 0    ||  ptime->month > 12  ||
       ptime->day   == 0    ||  ptime->day   > 31  ||
       ptime->hour  > 23    ||  ptime->minute > 59 ||
       ptime->second > 60)
      return 0;

   {
      size_t pos = 0;
      char number_buf[5]; /* enough for a four-digit year */

#     define APPEND_STRING(string) pos = png_safecat(out, 29, pos, (string))
#     define APPEND_NUMBER(format, value)\
         APPEND_STRING(PNG_FORMAT_NUMBER(number_buf, format, (value)))
#     define APPEND(ch) if (pos < 28) out[pos++] = (ch)

      APPEND_NUMBER(PNG_NUMBER_FORMAT_u, (unsigned)ptime->day);
      APPEND(' ');
      APPEND_STRING(short_months[(ptime->month - 1)]);
      APPEND(' ');
      APPEND_NUMBER(PNG_NUMBER_FORMAT_u, ptime->year);
      APPEND(' ');
      APPEND_NUMBER(PNG_NUMBER_FORMAT_02u, (unsigned)ptime->hour);
      APPEND(':');
      APPEND_NUMBER(PNG_NUMBER_FORMAT_02u, (unsigned)ptime->minute);
      APPEND(':');
      APPEND_NUMBER(PNG_NUMBER_FORMAT_02u, (unsigned)ptime->second);
      APPEND_STRING(" +0000"); /* This reliably terminates the buffer */
      PNG_UNUSED (pos)

#     undef APPEND
#     undef APPEND_NUMBER
#     undef APPEND_STRING
   }

   return 1;
}