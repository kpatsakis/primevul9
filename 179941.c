static inline double DecodeGamma(const double x)
{
  div_t
    quotient;

  double
    p,
    term[9];

  int
    exponent;

  static const double coefficient[] =  /* terms for x^(7/5), x=1.5 */
  {
    1.7917488588043277509,
    0.82045614371976854984,
    0.027694100686325412819,
    -0.00094244335181762134018,
    0.000064355540911469709545,
    -5.7224404636060757485e-06,
    5.8767669437311184313e-07,
    -6.6139920053589721168e-08,
    7.9323242696227458163e-09
  };

  static const double powers_of_two[] =  /* (2^x)^(7/5) */
  {
    1.0,
    2.6390158215457883983,
    6.9644045063689921093,
    1.8379173679952558018e+01,
    4.8502930128332728543e+01
  };

  /*
    Compute x^2.4 == x*x^(7/5) == pow(x,2.4).
  */
  term[0]=1.0;
  term[1]=4.0*frexp(x,&exponent)-3.0;
  term[2]=2.0*term[1]*term[1]-term[0];
  term[3]=2.0*term[1]*term[2]-term[1];
  term[4]=2.0*term[1]*term[3]-term[2];
  term[5]=2.0*term[1]*term[4]-term[3];
  term[6]=2.0*term[1]*term[5]-term[4];
  term[7]=2.0*term[1]*term[6]-term[5];
  term[8]=2.0*term[1]*term[7]-term[6];
  p=coefficient[0]*term[0]+coefficient[1]*term[1]+coefficient[2]*term[2]+
    coefficient[3]*term[3]+coefficient[4]*term[4]+coefficient[5]*term[5]+
    coefficient[6]*term[6]+coefficient[7]*term[7]+coefficient[8]*term[8];
  quotient=div(exponent-1,5);
  if (quotient.rem < 0)
    {
      quotient.quot-=1;
      quotient.rem+=5;
    }
  return(x*ldexp(powers_of_two[quotient.rem]*p,7*quotient.quot));
}