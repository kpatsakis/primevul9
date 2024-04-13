inline LutOutT lut_lookup_with_interpolation(int16_t value,
                                             const LutOutT* lut) {
  static_assert(std::is_same<LutOutT, int8_t>::value ||
                    std::is_same<LutOutT, int16_t>::value,
                "Only LUTs with int8 or int16 outputs are supported.");
  // 512 base values, lut[513] is only used to calculate the slope
  const uint16_t index = static_cast<uint16_t>(256 + (value >> 7));
  assert(index < 512 && "LUT index out of range.");
  const int16_t offset = value & 0x7f;

  // Base and slope are Q0.x
  const LutOutT base = lut[index];
  const LutOutT slope = lut[index + 1] - lut[index];

  // Q0.x * Q0.7 = Q0.(x + 7)
  // Round and convert from Q0.(x + 7) to Q0.x
  const int delta = (slope * offset + 64) >> 7;

  // Q0.15 + Q0.15
  return static_cast<LutOutT>(base + delta);
}