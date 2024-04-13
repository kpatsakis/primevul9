lzss_pointer_for_position(struct lzss *lzss, int64_t pos)
{
  return &lzss->window[lzss_offset_for_position(lzss, pos)];
}