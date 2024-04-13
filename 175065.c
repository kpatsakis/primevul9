int Fraction::round_up() const
{
  return (numerator + denominator - 1)/denominator;
}