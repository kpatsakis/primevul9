inline int32_t MultiplyByQuantizedMultiplier(int64_t x,
                                             int32_t quantized_multiplier,
                                             int shift) {
  // Inputs:
  // - quantized_multiplier has fixed point at bit 31
  // - shift is -31 to +7 (negative for right shift)
  //
  // Assumptions: The following input ranges are assumed
  // - quantize_scale>=0  (the usual range is (1<<30) to (1>>31)-1)
  // - scaling is chosen so final scaled result fits in int32_t
  // - input x is in the range -(1<<47) <= x < (1<<47)
  TFLITE_DCHECK(quantized_multiplier >= 0);
  TFLITE_DCHECK(shift >= -31 && shift < 8);
  TFLITE_DCHECK(x >= -(static_cast<int64_t>(1) << 47) &&
                x < (static_cast<int64_t>(1) << 47));

  const int32_t reduced_multiplier =
      (quantized_multiplier < 0x7FFF0000)
          ? ((quantized_multiplier + (1 << 15)) >> 16)
          : 0x7FFF;
  const int64_t total_shift = 15 - shift;
  const int64_t round = static_cast<int64_t>(1) << (total_shift - 1);
  int64_t result = x * static_cast<int64_t>(reduced_multiplier) + round;
  result = result >> total_shift;

  TFLITE_DCHECK(result >= std::numeric_limits<int32_t>::min() &&
                result <= std::numeric_limits<int32_t>::max());
  return static_cast<int32_t>(result);
}