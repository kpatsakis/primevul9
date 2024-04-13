inline void DownScaleInt32ToInt16Multiplier(int32_t multiplier_int32_t,
                                            int16_t* multiplier_int16_t) {
  TFLITE_DCHECK_GE(multiplier_int32_t, 0);
  static constexpr int32_t kRoundingOffset = 1 << 15;
  if (multiplier_int32_t >=
      std::numeric_limits<int32_t>::max() - kRoundingOffset) {
    *multiplier_int16_t = std::numeric_limits<int16_t>::max();
    return;
  }
  const int32_t result = (multiplier_int32_t + kRoundingOffset) >> 16;
  TFLITE_DCHECK_LE(result << 16, multiplier_int32_t + kRoundingOffset);
  TFLITE_DCHECK_GT(result << 16, multiplier_int32_t - kRoundingOffset);
  *multiplier_int16_t = result;
  TFLITE_DCHECK_EQ(*multiplier_int16_t, result);
}