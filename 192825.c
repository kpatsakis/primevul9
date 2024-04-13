png_icc_tag_char(png_uint_32 byte)
{
   byte &= 0xff;
   if (byte >= 32 && byte <= 126)
      return (char)byte;
   else
      return '?';
}