Fraction Fraction::operator/(int v) const
{
  return Fraction { numerator, denominator*v };
}