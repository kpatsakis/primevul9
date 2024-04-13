int CountLeadingZeros(T integer_input) {
  static_assert(std::is_unsigned<T>::value,
                "Only unsigned integer types handled.");
#if defined(__GNUC__)
  return integer_input ? __builtin_clz(integer_input)
                       : std::numeric_limits<T>::digits;
#else
  if (integer_input == 0) {
    return std::numeric_limits<T>::digits;
  }

  const T one_in_leading_positive = static_cast<T>(1)
                                    << (std::numeric_limits<T>::digits - 1);
  int leading_zeros = 0;
  while (integer_input < one_in_leading_positive) {
    integer_input <<= 1;
    ++leading_zeros;
  }
  return leading_zeros;
#endif
}