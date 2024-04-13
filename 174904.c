lzss_current_pointer(struct lzss *lzss)
{
  return lzss_pointer_for_position(lzss, lzss->position);
}