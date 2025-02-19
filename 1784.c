inline int32_t MultiplyByQuantizedMultiplierGreaterThanOne(
    int32_t x, int32_t quantized_multiplier, int shift) {
  TFLITE_DCHECK_GE(shift, 0);
  return MultiplyByQuantizedMultiplier(x, quantized_multiplier, shift);
}