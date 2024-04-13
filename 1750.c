inline int32_t MultiplyByQuantizedMultiplier(int32_t x,
                                             int32_t quantized_multiplier,
                                             int shift) {
  TFLITE_DCHECK(quantized_multiplier >= 0);
  TFLITE_DCHECK(shift >= -31 && shift <= 30);

  const int64_t total_shift = 31 - shift;
  const int64_t round = static_cast<int64_t>(1) << (total_shift - 1);
  int64_t result = x * static_cast<int64_t>(quantized_multiplier) + round;
  result = result >> total_shift;

  TFLITE_DCHECK(result >= std::numeric_limits<int32_t>::min() &&
                result <= std::numeric_limits<int32_t>::max());
  return static_cast<int32_t>(result);
}