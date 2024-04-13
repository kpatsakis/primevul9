inline int32x4x4_t MultiplyByQuantizedMultiplier4Rows(
    int32x4x4_t input_val, int32_t quantized_multiplier, int shift) {
  const int left_shift = std::max(shift, 0);
  const int right_shift = std::min(shift, 0);
  int32x4x4_t result;

  int32x4_t multiplier_dup = vdupq_n_s32(quantized_multiplier);
  int32x4_t left_shift_dup = vdupq_n_s32(left_shift);
  int32x4_t right_shift_dup = vdupq_n_s32(right_shift);

  result.val[0] =
      vrshlq_s32(vqrdmulhq_s32(vshlq_s32(input_val.val[0], left_shift_dup),
                               multiplier_dup),
                 right_shift_dup);

  result.val[1] =
      vrshlq_s32(vqrdmulhq_s32(vshlq_s32(input_val.val[1], left_shift_dup),
                               multiplier_dup),
                 right_shift_dup);

  result.val[2] =
      vrshlq_s32(vqrdmulhq_s32(vshlq_s32(input_val.val[2], left_shift_dup),
                               multiplier_dup),
                 right_shift_dup);

  result.val[3] =
      vrshlq_s32(vqrdmulhq_s32(vshlq_s32(input_val.val[3], left_shift_dup),
                               multiplier_dup),
                 right_shift_dup);

  return result;
}