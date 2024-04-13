inline int32_t MultiplyByQuantizedMultiplierSmallerThanOneExp(
    int32_t x, int32_t quantized_multiplier, int left_shift) {
  using gemmlowp::RoundingDivideByPOT;
  using gemmlowp::SaturatingRoundingDoublingHighMul;
  return RoundingDivideByPOT(
      SaturatingRoundingDoublingHighMul(x, quantized_multiplier), -left_shift);
}