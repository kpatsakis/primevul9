static inline void ModulateHCL(const double percent_hue,
  const double percent_chroma,const double percent_luma,double *red,
  double *green,double *blue)
{
  double
    hue,
    luma,
    chroma;

  /*
    Increase or decrease color luma, chroma, or hue.
  */
  ConvertRGBToHCL(*red,*green,*blue,&hue,&chroma,&luma);
  hue+=0.5*(0.01*percent_hue-1.0);
  chroma*=0.01*percent_chroma;
  luma*=0.01*percent_luma;
  ConvertHCLToRGB(hue,chroma,luma,red,green,blue);
}