inline Integer FloorLog2(Integer n) {
  static_assert(std::is_integral<Integer>::value, "");
  static_assert(std::is_signed<Integer>::value, "");
  static_assert(sizeof(Integer) == 4 || sizeof(Integer) == 8, "");
  TFLITE_CHECK_GT(n, 0);
  if (sizeof(Integer) == 4) {
    return 30 - CountLeadingSignBits(n);
  } else {
    return 62 - CountLeadingSignBits(n);
  }
}