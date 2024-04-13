IntegerType SaturatingRoundingMultiplyByPOTParam(IntegerType x, int exponent) {
  if (exponent == 0) {
    return x;
  }
  using ScalarIntegerType =
      typename gemmlowp::FixedPointRawTypeTraits<IntegerType>::ScalarRawType;
  const IntegerType min =
      gemmlowp::Dup<IntegerType>(std::numeric_limits<ScalarIntegerType>::min());
  const IntegerType max =
      gemmlowp::Dup<IntegerType>(std::numeric_limits<ScalarIntegerType>::max());
  const int ScalarIntegerTypeBits = 8 * sizeof(ScalarIntegerType);

  const std::int32_t threshold =
      ((1 << (ScalarIntegerTypeBits - 1 - exponent)) - 1);
  const IntegerType positive_mask =
      gemmlowp::MaskIfGreaterThan(x, gemmlowp::Dup<IntegerType>(threshold));
  const IntegerType negative_mask =
      gemmlowp::MaskIfLessThan(x, gemmlowp::Dup<IntegerType>(-threshold));

  IntegerType result = gemmlowp::ShiftLeft(x, exponent);
  result = gemmlowp::SelectUsingMask(positive_mask, max, result);
  result = gemmlowp::SelectUsingMask(negative_mask, min, result);
  return result;
}