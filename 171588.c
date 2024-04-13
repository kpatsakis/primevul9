static inline unsigned short SinglePrecisionToHalf(const float value)
{
  typedef union _SinglePrecision
  {
    unsigned int
      fixed_point;

    float
      single_precision;
  } SinglePrecision;

  register int
    exponent;

  register unsigned int
    significand,
    sign_bit;

  SinglePrecision
    map;

  unsigned short
    half;

  /*
    The IEEE 754 standard specifies half precision as having:

      Sign bit: 1 bit
      Exponent width: 5 bits
      Significand precision: 11 (10 explicitly stored)
  */
  map.single_precision=value;
  sign_bit=(map.fixed_point >> 16) & 0x00008000;
  exponent=(int) ((map.fixed_point >> ExponentShift) & 0x000000ff)-ExponentBias;
  significand=map.fixed_point & 0x007fffff;
  if (exponent <= 0)
    {
      int
        shift;

      if (exponent < -10)
        return((unsigned short) sign_bit);
      significand=significand | 0x00800000;
      shift=(int) (14-exponent);
      significand=(unsigned int) ((significand+((1 << (shift-1))-1)+
        ((significand >> shift) & 0x01)) >> shift);
      return((unsigned short) (sign_bit | significand));
    }
  else
    if (exponent == (0xff-ExponentBias))
      {
        if (significand == 0)
          return((unsigned short) (sign_bit | ExponentMask));
        else
          {
            significand>>=SignificandShift;
            half=(unsigned short) (sign_bit | significand |
              (significand == 0) | ExponentMask);
            return(half);
          }
      }
  significand=significand+((significand >> SignificandShift) & 0x01)+0x00000fff;
  if ((significand & 0x00800000) != 0)
    {
      significand=0;
      exponent++;
    }
  if (exponent > 30)
    {
      float
        alpha;

      register int
        i;

      /*
        Float overflow.
      */
      alpha=1.0e10;
      for (i=0; i < 10; i++)
        alpha*=alpha;
      return((unsigned short) (sign_bit | ExponentMask));
    }
  half=(unsigned short) (sign_bit | (exponent << 10) |
    (significand >> SignificandShift));
  return(half);
}