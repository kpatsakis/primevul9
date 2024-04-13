log_x_for_x_greater_than_or_equal_to_1_impl(
    gemmlowp::FixedPoint<int32_t, InputIntegerBits> input_val) {
  // assert(__builtin_clz(0u) >= std::numeric_limits<uint32_t>::digits - 1);
  // assert(__builtin_clz(0u) <= std::numeric_limits<uint32_t>::digits);
  using FixedPoint0 = gemmlowp::FixedPoint<int32_t, 0>;
  // The reason for accumulating the result with an extra bit of headroom is
  // that z_pow_2_adj * log_2 might be saturated, and adding num_scaled *
  // recip_denom will otherwise introduce an error.
  static constexpr int kAccumIntegerBits = OutputIntegerBits + 1;
  using FixedPointAccum = gemmlowp::FixedPoint<int32_t, kAccumIntegerBits>;

  const FixedPoint0 log_2 = GEMMLOWP_CHECKED_FIXEDPOINT_CONSTANT(
      FixedPoint0, 1488522236, std::log(2.0));
  const FixedPoint0 sqrt_sqrt_half = GEMMLOWP_CHECKED_FIXEDPOINT_CONSTANT(
      FixedPoint0, 1805811301, std::sqrt(std::sqrt(0.5)));
  const FixedPoint0 sqrt_half = GEMMLOWP_CHECKED_FIXEDPOINT_CONSTANT(
      FixedPoint0, 1518500250, std::sqrt(0.5));
  const FixedPoint0 one_quarter =
      GEMMLOWP_CHECKED_FIXEDPOINT_CONSTANT(FixedPoint0, 536870912, 1.0 / 4.0);

  const FixedPoint0 alpha_n = GEMMLOWP_CHECKED_FIXEDPOINT_CONSTANT(
      FixedPoint0, 117049297, 11.0 / 240.0 * std::sqrt(std::sqrt(2.0)));
  const FixedPoint0 alpha_d = GEMMLOWP_CHECKED_FIXEDPOINT_CONSTANT(
      FixedPoint0, 127690142, 1.0 / 20.0 * std::sqrt(std::sqrt(2.0)));
  const FixedPoint0 alpha_i = GEMMLOWP_CHECKED_FIXEDPOINT_CONSTANT(
      FixedPoint0, 1057819769,
      2.0 / std::sqrt(std::sqrt(2.0)) - std::sqrt(std::sqrt(2.0)));
  const FixedPoint0 alpha_f = GEMMLOWP_CHECKED_FIXEDPOINT_CONSTANT(
      FixedPoint0, 638450708, 1.0 / 4.0 * std::sqrt(std::sqrt(2.0)));

  const FixedPointAccum shifted_quarter =
      gemmlowp::Rescale<kAccumIntegerBits>(one_quarter);

  // Reinterpret the input value as Q0.31, because we will figure out the
  // required shift "ourselves" instead of using, say, Rescale.
  FixedPoint0 z_a = FixedPoint0::FromRaw(input_val.raw());
  // z_a_pow_2 = input_integer_bits - z_a_headroom;
  int z_a_headroom_plus_1 = CountLeadingZeros(static_cast<uint32_t>(z_a.raw()));
  FixedPoint0 r_a_tmp =
      SaturatingRoundingMultiplyByPOTParam(z_a, (z_a_headroom_plus_1 - 1));
  const int32_t r_a_raw =
      SaturatingRoundingMultiplyByPOTParam((r_a_tmp * sqrt_half).raw(), 1);
  // z_pow_2_adj = max(z_pow_2_a - 0.75, z_pow_2_b - 0.25);
  // z_pow_2_adj = max(InputIntegerBits - z_a_headroom_plus_1 + 0.25,
  //                   InputIntegerBits - z_b_headroom - 0.25);
  const FixedPointAccum z_a_pow_2_adj = SaturatingAddNonGemmlowp(
      FixedPointAccum::FromRaw(SaturatingRoundingMultiplyByPOTParam(
          static_cast<int32_t>(InputIntegerBits - z_a_headroom_plus_1),
          31 - kAccumIntegerBits)),
      shifted_quarter);

  // z_b is treated like z_a, but premultiplying by sqrt(0.5).
  FixedPoint0 z_b = z_a * sqrt_half;
  int z_b_headroom = CountLeadingZeros(static_cast<uint32_t>(z_b.raw())) - 1;
  const int32_t r_b_raw =
      SaturatingRoundingMultiplyByPOTParam(z_a.raw(), z_b_headroom);
  const FixedPointAccum z_b_pow_2_adj = SaturatingSub(
      FixedPointAccum::FromRaw(SaturatingRoundingMultiplyByPOTParam(
          static_cast<int32_t>(InputIntegerBits - z_b_headroom),
          31 - kAccumIntegerBits)),
      shifted_quarter);

  const FixedPoint0 r = FixedPoint0::FromRaw(std::min(r_a_raw, r_b_raw));
  const FixedPointAccum z_pow_2_adj = FixedPointAccum::FromRaw(
      std::max(z_a_pow_2_adj.raw(), z_b_pow_2_adj.raw()));

  const FixedPoint0 p = gemmlowp::RoundingHalfSum(r, sqrt_sqrt_half);
  FixedPoint0 q = r - sqrt_sqrt_half;
  q = q + q;

  const FixedPoint0 common_sq = q * q;
  const FixedPoint0 num = q * r + q * common_sq * alpha_n;
  const FixedPoint0 denom_minus_one_0 =
      p * (alpha_i + q + alpha_d * common_sq) + alpha_f * q;
  const FixedPoint0 recip_denom =
      one_over_one_plus_x_for_x_in_0_1(denom_minus_one_0);

  const FixedPointAccum num_scaled = gemmlowp::Rescale<kAccumIntegerBits>(num);
  return gemmlowp::Rescale<OutputIntegerBits>(z_pow_2_adj * log_2 +
                                              num_scaled * recip_denom);
}