lzss_current_offset(struct lzss *lzss)
{
  return lzss_offset_for_position(lzss, lzss->position);
}