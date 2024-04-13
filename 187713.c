static double GetUserSpaceCoordinateValue(const SVGInfo *svg_info,int type,
  const char *string)
{
  char
    *next_token,
    token[MaxTextExtent];

  const char
    *p;

  double
    value;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",string);
  assert(string != (const char *) NULL);
  p=(const char *) string;
  (void) GetNextToken(p,&p,MaxTextExtent,token);
  value=StringToDouble(token,&next_token);
  if (strchr(token,'%') != (char *) NULL)
    {
      double
        alpha,
        beta;

      if (type > 0)
        {
          if (svg_info->view_box.width == 0.0)
            return(0.0);
          return(svg_info->view_box.width*value/100.0);
        }
      if (type < 0)
        {
          if (svg_info->view_box.height == 0.0)
            return(0.0);
          return(svg_info->view_box.height*value/100.0);
        }
      alpha=value-svg_info->view_box.width;
      beta=value-svg_info->view_box.height;
      return(hypot(alpha,beta)/sqrt(2.0)/100.0);
    }
  (void) GetNextToken(p,&p,MaxTextExtent,token);
  if (LocaleNCompare(token,"cm",2) == 0)
    return(DefaultSVGDensity*svg_info->scale[0]/2.54*value);
  if (LocaleNCompare(token,"em",2) == 0)
    return(svg_info->pointsize*value);
  if (LocaleNCompare(token,"ex",2) == 0)
    return(svg_info->pointsize*value/2.0);
  if (LocaleNCompare(token,"in",2) == 0)
    return(DefaultSVGDensity*svg_info->scale[0]*value);
  if (LocaleNCompare(token,"mm",2) == 0)
    return(DefaultSVGDensity*svg_info->scale[0]/25.4*value);
  if (LocaleNCompare(token,"pc",2) == 0)
    return(DefaultSVGDensity*svg_info->scale[0]/6.0*value);
  if (LocaleNCompare(token,"pt",2) == 0)
    return(svg_info->scale[0]*value);
  if (LocaleNCompare(token,"px",2) == 0)
    return(value);
  return(value);
}