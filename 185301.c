MATCHER_P(Percent, rhs, "") {
  envoy::type::v3::FractionalPercent expected;
  expected.set_numerator(rhs);
  expected.set_denominator(envoy::type::v3::FractionalPercent::HUNDRED);
  return TestUtility::protoEqual(expected, arg, /*ignore_repeated_field_ordering=*/false);
}