static inline void ModulateHSI(const double percent_hue,
  const double percent_saturation,const double percent_intensity,double *red,
  double *green,double *blue)
{
  double
    intensity,
    hue,
    saturation;

  /*
    Increase or decrease color intensity, saturation, or hue.
  */
  ConvertRGBToHSI(*red,*green,*blue,&hue,&saturation,&intensity);
  hue+=0.5*(0.01*percent_hue-1.0);
  saturation*=0.01*percent_saturation;
  intensity*=0.01*percent_intensity;
  ConvertHSIToRGB(hue,saturation,intensity,red,green,blue);
}