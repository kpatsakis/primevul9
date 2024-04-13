static inline float HalfToSinglePrecision(const unsigned short half)
{
#define ExponentBias  (127-15)
#define ExponentMask  0x7c00
#define ExponentShift  23
#define SignBitShift  31
#define SignificandShift  13
#define SignificandMask  0x00000400

  typedef union _SinglePrecision
  {
    unsigned int
      fixed_point;

    float
      single_precision;
  } SinglePrecision;

  register unsigned int
    exponent,
    significand,
    sign_bit;

  SinglePrecision
    map;

  unsigned int
    value;

  /*
    The IEEE 754 standard specifies half precision as having:

      Sign bit: 1 bit
      Exponent width: 5 bits
      Significand precision: 11 (10 explicitly stored)
  */
  sign_bit=(unsigned int) ((half >> 15) & 0x00000001);
  exponent=(unsigned int) ((half >> 10) & 0x0000001f);
  significand=(unsigned int) (half & 0x000003ff);
  if (exponent == 0)
    {
      if (significand == 0)
        value=sign_bit << SignBitShift;
      else
        {
          while ((significand & SignificandMask) == 0)
          {
            significand<<=1;
            exponent--;
          }
          exponent++;
          significand&=(~SignificandMask);
          exponent+=ExponentBias;
          value=(sign_bit << SignBitShift) | (exponent << ExponentShift) |
            (significand << SignificandShift);
        }
    }
  else
    if (exponent == SignBitShift)
      {
        value=(sign_bit << SignBitShift) | 0x7f800000;
        if (significand != 0)
          value|=(significand << SignificandShift);
      }
    else
      {
        exponent+=ExponentBias;
        significand<<=SignificandShift;
        value=(sign_bit << SignBitShift) | (exponent << ExponentShift) |
          significand;
      }
  map.fixed_point=value;
  return(map.single_precision);
}