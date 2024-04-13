static inline Quantum ReadVIPSPixelNONE(Image *image,
  const VIPSBandFormat format,const VIPSType type)
{
  switch(type)
  {
    case VIPSTypeB_W:
    case VIPSTypeRGB:
      {
        unsigned char
          c;

        switch(format)
        {
          case VIPSBandFormatUCHAR:
          case VIPSBandFormatCHAR:
            c=(unsigned char) ReadBlobByte(image);
            break;
          case VIPSBandFormatUSHORT:
          case VIPSBandFormatSHORT:
            c=(unsigned char) ReadBlobShort(image);
            break;
          case VIPSBandFormatUINT:
          case VIPSBandFormatINT:
            c=(unsigned char) ReadBlobLong(image);
            break;
          case VIPSBandFormatFLOAT:
            c=(unsigned char) ReadBlobFloat(image);
            break;
          case VIPSBandFormatDOUBLE:
            c=(unsigned char) ReadBlobDouble(image);
            break;
          default:
            c=0;
            break;
        }
        return(ScaleCharToQuantum(c));
      }
    case VIPSTypeGREY16:
    case VIPSTypeRGB16:
      {
        unsigned short
          s;

        switch(format)
        {
          case VIPSBandFormatUSHORT:
          case VIPSBandFormatSHORT:
            s=(unsigned short) ReadBlobShort(image);
            break;
          case VIPSBandFormatUINT:
          case VIPSBandFormatINT:
            s=(unsigned short) ReadBlobLong(image);
            break;
          case VIPSBandFormatFLOAT:
            s=(unsigned short) ReadBlobFloat(image);
            break;
          case VIPSBandFormatDOUBLE:
            s=(unsigned short) ReadBlobDouble(image);
            break;
          default:
            s=0;
            break;
        }
        return(ScaleShortToQuantum(s));
      }
    case VIPSTypeCMYK:
    case VIPSTypesRGB:
      switch(format)
      {
        case VIPSBandFormatUCHAR:
        case VIPSBandFormatCHAR:
          return(ScaleCharToQuantum((unsigned char) ReadBlobByte(image)));
        case VIPSBandFormatUSHORT:
        case VIPSBandFormatSHORT:
          return(ScaleShortToQuantum(ReadBlobShort(image)));
        case VIPSBandFormatUINT:
        case VIPSBandFormatINT:
          return(ScaleLongToQuantum(ReadBlobLong(image)));
        case VIPSBandFormatFLOAT:
          return((Quantum) ((float) QuantumRange*(ReadBlobFloat(image)/1.0)));
        case VIPSBandFormatDOUBLE:
          return((Quantum) ((double) QuantumRange*(ReadBlobDouble(
            image)/1.0)));
        default:
          return((Quantum) 0);
      }
    default:
      return((Quantum) 0);
  }
}