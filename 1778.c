inline int16_t lut_lookup(int8_t value, const int16_t* lut) {
  return lut[128 + value];
}