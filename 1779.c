inline int32_t MultiplyByQuantizedMultiplierGreaterThanOne(
    int32_t x, int32_t quantized_multiplier, int left_shift) {
  using gemmlowp::SaturatingRoundingDoublingHighMul;
  return SaturatingRoundingDoublingHighMul(x * (1 << left_shift),
                                           quantized_multiplier);
}