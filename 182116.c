MagickExport char *XGetScreenDensity(Display *display)
{
  char
    density[MaxTextExtent];

  double
    x_density,
    y_density;

  /*
    Set density as determined by screen size.
  */
  x_density=((((double) DisplayWidth(display,XDefaultScreen(display)))*25.4)/
    ((double) DisplayWidthMM(display,XDefaultScreen(display))));
  y_density=((((double) DisplayHeight(display,XDefaultScreen(display)))*25.4)/
    ((double) DisplayHeightMM(display,XDefaultScreen(display))));
  (void) FormatLocaleString(density,MaxTextExtent,"%gx%g",x_density,
    y_density);
  return(GetPageGeometry(density));
}