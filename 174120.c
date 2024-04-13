static inline void ModulateLCHuv(const double percent_luma,
  const double percent_chroma,const double percent_hue,double *red,
  double *green,double *blue)
{
  double
    hue,
    luma,
    chroma;

  /*
    Increase or decrease color luma, chroma, or hue.
  */
  ConvertRGBToLCHuv(*red,*green,*blue,&luma,&chroma,&hue);
  luma*=0.01*percent_luma;
  chroma*=0.01*percent_chroma;
  hue+=0.5*(0.01*percent_hue-1.0);
  ConvertLCHuvToRGB(luma,chroma,hue,red,green,blue);
}