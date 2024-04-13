Fraction Fraction::operator-(int v) const
{
  return Fraction { numerator - v * denominator, denominator };
}