gemmlowp::FixedPoint<tRawType, tIntegerBits> SaturatingAddNonGemmlowp(
    gemmlowp::FixedPoint<tRawType, tIntegerBits> a,
    gemmlowp::FixedPoint<tRawType, tIntegerBits> b) {
  return gemmlowp::FixedPoint<tRawType, tIntegerBits>::FromRaw(
      SaturatingAddNonGemmlowp(a.raw(), b.raw()));
}