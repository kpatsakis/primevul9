static void ConvertYUVToRGB(const double Y,const double U,const double V,
  double *red,double *green,double *blue)
{
  *red=QuantumRange*(Y-3.945707070708279e-05*(U-0.5)+1.1398279671717170825*
    (V-0.5));
  *green=QuantumRange*(Y-0.3946101641414141437*(U-0.5)-0.5805003156565656797*
    (V-0.5));
  *blue=QuantumRange*(Y+2.0319996843434342537*(U-0.5)-4.813762626262513e-04*
    (V-0.5));
}