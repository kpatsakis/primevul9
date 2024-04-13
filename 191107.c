static inline MagickBooleanType IsSupportedCombination(
  const VIPSBandFormat format,const VIPSType type)
{
  switch(type)
  {
    case VIPSTypeB_W:
    case VIPSTypeCMYK:
    case VIPSTypeRGB:
    case VIPSTypesRGB:
      return(MagickTrue);
    case VIPSTypeGREY16:
    case VIPSTypeRGB16:
      switch(format)
      {
        case VIPSBandFormatUSHORT:
        case VIPSBandFormatSHORT:
        case VIPSBandFormatUINT:
        case VIPSBandFormatINT:
        case VIPSBandFormatFLOAT:
        case VIPSBandFormatDOUBLE:
          return(MagickTrue);
        default:
          return(MagickFalse);
      }
    default:
      return(MagickFalse);
  }
}