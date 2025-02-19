constexpr int lut_size() {
  static_assert(std::is_same<LutInT, int8_t>::value ||
                    std::is_same<LutInT, int16_t>::value,
                "Only LUTs with int8 or int16 inputs are supported.");
  return std::is_same<LutInT, int8_t>::value ? 256 : 513;
}