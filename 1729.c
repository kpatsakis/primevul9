Integer RoundUp(Integer i) {
  return RoundDown<Modulus>(i + Modulus - 1);
}