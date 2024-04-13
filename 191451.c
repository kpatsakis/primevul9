static void ConvertYIQToRGB(const double Y,const double I,const double Q,
  double *red,double *green,double *blue)
{
  *red=QuantumRange*(Y+0.9562957197589482261*(I-0.5)+0.6210244164652610754*
    (Q-0.5));
  *green=QuantumRange*(Y-0.2721220993185104464*(I-0.5)-0.6473805968256950427*
    (Q-0.5));
  *blue=QuantumRange*(Y-1.1069890167364901945*(I-0.5)+1.7046149983646481374*
    (Q-0.5));
}