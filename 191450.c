static void ConvertYPbPrToRGB(const double Y,const double Pb,const double Pr,
  double *red,double *green,double *blue)
{
  *red=QuantumRange*(0.99999999999914679361*Y-1.2188941887145875e-06*(Pb-0.5)+
    1.4019995886561440468*(Pr-0.5));
  *green=QuantumRange*(0.99999975910502514331*Y-0.34413567816504303521*(Pb-0.5)-
    0.71413649331646789076*(Pr-0.5));
  *blue=QuantumRange*(1.00000124040004623180*Y+1.77200006607230409200*(Pb-0.5)+
    2.1453384174593273e-06*(Pr-0.5));
}