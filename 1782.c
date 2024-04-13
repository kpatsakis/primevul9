inline int16_t lut_lookup(int16_t value, const int16_t* lut) {
  return lut_lookup_with_interpolation(value, lut);
}