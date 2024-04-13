lzss_offset_for_position(struct lzss *lzss, int64_t pos)
{
  return (int)(pos & lzss->mask);
}