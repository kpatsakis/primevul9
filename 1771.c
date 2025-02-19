inline int32_t MultiplyByQuantizedMultiplierSmallerThanOneExp(
    int32_t x, int32_t quantized_multiplier, int shift) {
  TFLITE_DCHECK_LE(shift, 0);
  return MultiplyByQuantizedMultiplier(x, quantized_multiplier, shift);
}