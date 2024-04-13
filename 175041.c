int Fraction::round() const
{
  return (numerator + denominator/2)/denominator;
}