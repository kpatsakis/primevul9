inline void GetInvSqrtQuantizedMultiplierExp(int32_t input, int reverse_shift,
                                             int32_t* output_inv_sqrt,
                                             int* output_shift) {
  TFLITE_DCHECK_GE(input, 0);
  if (input <= 1) {
    // Handle the input value 1 separately to avoid overflow in that case
    // in the general computation below (b/143972021). Also handle 0 as if it
    // were a 1. 0 is an invalid input here (divide by zero) and 1 is a valid
    // but rare/unrealistic input value. We can expect both to occur in some
    // incompletely trained models, but probably not in fully trained models.
    *output_inv_sqrt = std::numeric_limits<std::int32_t>::max();
    *output_shift = 0;
    return;
  }
  TFLITE_DCHECK_GT(input, 1);
  *output_shift = 11;
  while (input >= (1 << 29)) {
    input /= 4;
    ++*output_shift;
  }
  const unsigned max_left_shift_bits =
      CountLeadingZeros(static_cast<uint32_t>(input)) - 1;
  const unsigned max_left_shift_bit_pairs = max_left_shift_bits / 2;
  const unsigned left_shift_bit_pairs = max_left_shift_bit_pairs - 1;
  *output_shift -= left_shift_bit_pairs;
  input <<= 2 * left_shift_bit_pairs;
  TFLITE_DCHECK_GE(input, (1 << 27));
  TFLITE_DCHECK_LT(input, (1 << 29));
  using gemmlowp::FixedPoint;
  using gemmlowp::Rescale;
  using gemmlowp::SaturatingRoundingMultiplyByPOT;
  // Using 3 integer bits gives us enough room for the internal arithmetic in
  // this Newton-Raphson iteration.
  using F3 = FixedPoint<int32_t, 3>;
  using F0 = FixedPoint<int32_t, 0>;
  const F3 fixedpoint_input = F3::FromRaw(input >> 1);
  const F3 fixedpoint_half_input =
      SaturatingRoundingMultiplyByPOT<-1>(fixedpoint_input);
  const F3 fixedpoint_half_three =
      GEMMLOWP_CHECKED_FIXEDPOINT_CONSTANT(F3, (1 << 28) + (1 << 27), 1.5);
  // Newton-Raphson iteration
  // Naive unoptimized starting guess: x = 1
  F3 x = F3::One();
  // Naive unoptimized number of iterations: 5
  for (int i = 0; i < 5; i++) {
    const F3 x3 = Rescale<3>(x * x * x);
    x = Rescale<3>(fixedpoint_half_three * x - fixedpoint_half_input * x3);
  }
  const F0 fixedpoint_half_sqrt_2 =
      GEMMLOWP_CHECKED_FIXEDPOINT_CONSTANT(F0, 1518500250, std::sqrt(2.) / 2.);
  x = x * fixedpoint_half_sqrt_2;
  *output_inv_sqrt = x.raw();
  if (*output_shift < 0) {
    *output_inv_sqrt <<= -*output_shift;
    *output_shift = 0;
  }
  // Convert right shift (right is positive) to left shift.
  *output_shift *= reverse_shift;
}