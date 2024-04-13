static inline unsigned char adjustComp(int r, int adjust, int depth, int sgndcorr, bool indexed) {
  if (!indexed) {
    r += sgndcorr;
    if (adjust) {
      r = (r >> adjust)+((r >> (adjust-1))%2);
    } else if (depth < 8) {
      r = r << (8 - depth);
    }
  }
  if (unlikely(r > 255))
    r = 255;
  return r;  
}