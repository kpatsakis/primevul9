static void ConvertYDbDrToRGB(const double Y,const double Db,const double Dr,
  double *red,double *green,double *blue)
{
  *red=QuantumRange*(Y+9.2303716147657e-05*(Db-0.5)-
    0.52591263066186533*(Dr-0.5));
  *green=QuantumRange*(Y-0.12913289889050927*(Db-0.5)+
    0.26789932820759876*(Dr-0.5));
  *blue=QuantumRange*(Y+0.66467905997895482*(Db-0.5)-
    7.9202543533108e-05*(Dr-0.5));
}