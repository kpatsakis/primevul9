Fraction Fraction::operator+(const Fraction& b) const
{
  if (denominator == b.denominator) {
    return Fraction { numerator + b.numerator, denominator };
  }
  else {
    return Fraction { numerator * b.denominator + b.numerator * denominator,
        denominator * b.denominator };
  }
}