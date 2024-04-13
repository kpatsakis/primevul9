static const char *XVisualClassName(const int visual_class)
{
  switch (visual_class)
  {
    case StaticGray: return("StaticGray");
    case GrayScale: return("GrayScale");
    case StaticColor: return("StaticColor");
    case PseudoColor: return("PseudoColor");
    case TrueColor: return("TrueColor");
    case DirectColor: return("DirectColor");
  }
  return("unknown visual class");
}