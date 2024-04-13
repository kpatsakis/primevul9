SaturatingRoundingMultiplyByPOTParam(
    gemmlowp::FixedPoint<tRawType, tIntegerBits> a, int exponent) {
  return gemmlowp::FixedPoint<tRawType, tIntegerBits>::FromRaw(
      SaturatingRoundingMultiplyByPOTParam(a.raw(), exponent));
}