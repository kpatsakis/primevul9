void TightDecoder::FilterPalette(const PIXEL_T* palette, int palSize,
                                 const rdr::U8* inbuf, PIXEL_T* outbuf,
                                 int stride, const Rect& r)
{
  // Indexed color
  int x, h = r.height(), w = r.width(), b, pad = stride - w;
  PIXEL_T* ptr = outbuf;
  rdr::U8 bits;
  const rdr::U8* srcPtr = inbuf;
  if (palSize <= 2) {
    // 2-color palette
    while (h > 0) {
      for (x = 0; x < w / 8; x++) {
        bits = *srcPtr++;
        for (b = 7; b >= 0; b--) {
          *ptr++ = palette[bits >> b & 1];
        }
      }
      if (w % 8 != 0) {
        bits = *srcPtr++;
        for (b = 7; b >= 8 - w % 8; b--) {
          *ptr++ = palette[bits >> b & 1];
        }
      }
      ptr += pad;
      h--;
    }
  } else {
    // 256-color palette
    while (h > 0) {
      PIXEL_T *endOfRow = ptr + w;
      while (ptr < endOfRow) {
        *ptr++ = palette[*srcPtr++];
      }
      ptr += pad;
      h--;
    }
  }
}