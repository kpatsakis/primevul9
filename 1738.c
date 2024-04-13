inline int8_t lut_lookup(int8_t value, const int8_t* lut) {
  return lut[128 + value];
}