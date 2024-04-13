static inline double EncodeGamma(const double x)
{
  div_t
    quotient;

  double
    p,
    term[9];

  int
    exponent;

  static const double coefficient[] =  /* Chebychevi poly: x^(5/12), x=1.5 */
  {
    1.1758200232996901923,
    0.16665763094889061230,
    -0.0083154894939042125035,
    0.00075187976780420279038,
    -0.000083240178519391795367,
    0.000010229209410070008679,
    -1.3400466409860246e-06,
    1.8333422241635376682e-07,
    -2.5878596761348859722e-08
  };

  static const double powers_of_two[] =  /* (2^N)^(5/12) */
  {
    1.0,
    1.3348398541700343678,
    1.7817974362806785482,
    2.3784142300054420538,
    3.1748021039363991669,
    4.2378523774371812394,
    5.6568542494923805819,
    7.5509945014535482244,
    1.0079368399158985525e1,
    1.3454342644059433809e1,
    1.7959392772949968275e1,
    2.3972913230026907883e1
  };

  /*
    Compute x^(1/2.4) == x^(5/12) == pow(x,1.0/2.4).
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
  quotient=div(exponent-1,12);
  if (quotient.rem < 0)
    {
      quotient.quot-=1;
      quotient.rem+=12;
    }
  return(ldexp(powers_of_two[quotient.rem]*p,5*quotient.quot));
}