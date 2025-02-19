lzss_emit_match(struct rar *rar, int offset, int length)
{
  int dstoffs = lzss_current_offset(&rar->lzss);
  int srcoffs = (dstoffs - offset) & lzss_mask(&rar->lzss);
  int l, li, remaining;
  unsigned char *d, *s;

  remaining = length;
  while (remaining > 0) {
    l = remaining;
    if (dstoffs > srcoffs) {
      if (l > lzss_size(&rar->lzss) - dstoffs)
        l = lzss_size(&rar->lzss) - dstoffs;
    } else {
      if (l > lzss_size(&rar->lzss) - srcoffs)
        l = lzss_size(&rar->lzss) - srcoffs;
    }
    d = &(rar->lzss.window[dstoffs]);
    s = &(rar->lzss.window[srcoffs]);
    if ((dstoffs + l < srcoffs) || (srcoffs + l < dstoffs))
      memcpy(d, s, l);
    else {
      for (li = 0; li < l; li++)
        d[li] = s[li];
    }
    remaining -= l;
    dstoffs = (dstoffs + l) & lzss_mask(&(rar->lzss));
    srcoffs = (srcoffs + l) & lzss_mask(&(rar->lzss));
  }
  rar->lzss.position += length;
}