parse_tag (unsigned char const **buffer, size_t *size, struct tag_info *ti)
{
  int c;
  unsigned long tag;
  const unsigned char *buf = *buffer;
  size_t length = *size;

  ti->length = 0;
  ti->ndef = 0;
  ti->nhdr = 0;

  /* Get the tag */
  if (!length)
    return -1; /* premature eof */
  c = *buf++; length--;
  ti->nhdr++;

  ti->class = (c & 0xc0) >> 6;
  ti->is_constructed = !!(c & 0x20);
  tag = c & 0x1f;

  if (tag == 0x1f)
    {
      tag = 0;
      do
        {
          tag <<= 7;
          if (!length)
            return -1; /* premature eof */
          c = *buf++; length--;
          ti->nhdr++;
          tag |= c & 0x7f;
        }
      while (c & 0x80);
    }
  ti->tag = tag;

  /* Get the length */
  if (!length)
    return -1; /* prematureeof */
  c = *buf++; length--;
  ti->nhdr++;

  if ( !(c & 0x80) )
    ti->length = c;
  else if (c == 0x80)
    ti->ndef = 1;
  else if (c == 0xff)
    return -1; /* forbidden length value */
  else
    {
      unsigned long len = 0;
      int count = c & 0x7f;

      for (; count; count--)
        {
          len <<= 8;
          if (!length)
            return -1; /* premature_eof */
          c = *buf++; length--;
          ti->nhdr++;
          len |= c & 0xff;
        }
      ti->length = len;
    }

  if (ti->class == UNIVERSAL && !ti->tag)
    ti->length = 0;

  if (ti->length > length)
    return -1; /* data larger than buffer. */

  *buffer = buf;
  *size = length;
  return 0;
}