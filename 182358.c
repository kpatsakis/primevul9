MagickExport size_t ExportQuantumPixels(const Image *image,
  const CacheView *image_view,const QuantumInfo *quantum_info,
  const QuantumType quantum_type,unsigned char *magick_restrict pixels,
  ExceptionInfo *exception)
{
  MagickRealType
    alpha;

  MagickSizeType
    number_pixels;

  const IndexPacket
    *magick_restrict indexes;

  const PixelPacket
    *magick_restrict p;

  ssize_t
    x;

  unsigned char
    *magick_restrict q;

  size_t
    extent;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(quantum_info != (QuantumInfo *) NULL);
  assert(quantum_info->signature == MagickCoreSignature);
  if (pixels == (unsigned char *) NULL)
    pixels=GetQuantumPixels(quantum_info);
  if (image_view == (CacheView *) NULL)
    {
      number_pixels=GetImageExtent(image);
      p=GetVirtualPixelQueue(image);
      indexes=GetVirtualIndexQueue(image);
    }
  else
    {
      number_pixels=GetCacheViewExtent(image_view);
      p=GetCacheViewVirtualPixelQueue(image_view);
      indexes=GetCacheViewVirtualIndexQueue(image_view);
    }
  if (quantum_info->alpha_type == AssociatedQuantumAlpha)
    {
      register PixelPacket
        *magick_restrict q;

      /*
        Associate alpha.
      */
      q=GetAuthenticPixelQueue(image);
      if (image_view != (CacheView *) NULL)
        q=(PixelPacket *) GetCacheViewVirtualPixelQueue(image_view);
      for (x=0; x < (ssize_t) image->columns; x++)
      {
        alpha=QuantumScale*GetPixelAlpha(q);
        SetPixelRed(q,ClampToQuantum(alpha*GetPixelRed(q)));
        SetPixelGreen(q,ClampToQuantum(alpha*GetPixelGreen(q)));
        SetPixelBlue(q,ClampToQuantum(alpha*GetPixelBlue(q)));
        q++;
      }
    }
  if ((quantum_type == CbYCrQuantum) || (quantum_type == CbYCrAQuantum))
    {
      Quantum
        quantum;

      register PixelPacket
        *magick_restrict q;

      q=GetAuthenticPixelQueue(image);
      if (image_view != (CacheView *) NULL)
        q=GetAuthenticPixelQueue(image);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        quantum=GetPixelRed(q);
        SetPixelRed(q,GetPixelGreen(q));
        SetPixelGreen(q,quantum);
        q++;
      }
    }
  x=0;
  q=pixels;
  ResetQuantumState((QuantumInfo *) quantum_info);
  extent=GetQuantumExtent(image,quantum_info,quantum_type);
  switch (quantum_type)
  {
    case AlphaQuantum:
    {
      ExportAlphaQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case BGRQuantum:
    {
      ExportBGRQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case BGRAQuantum:
    {
      ExportBGRAQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case BGROQuantum:
    {
      ExportBGROQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case BlackQuantum:
    {
      ExportBlackQuantum(image,(QuantumInfo *) quantum_info,number_pixels,p,
        indexes,q,exception);
      break;
    }
    case BlueQuantum:
    case YellowQuantum:
    {
      ExportBlueQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case CbYCrYQuantum:
    {
      ExportCbYCrYQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case CMYKQuantum:
    {
      ExportCMYKQuantum(image,(QuantumInfo *) quantum_info,number_pixels,p,
        indexes,q,exception);
      break;
    }
    case CMYKAQuantum:
    {
      ExportCMYKAQuantum(image,(QuantumInfo *) quantum_info,number_pixels,p,
        indexes,q,exception);
      break;
    }
    case CMYKOQuantum:
    {
      ExportCMYKOQuantum(image,(QuantumInfo *) quantum_info,number_pixels,p,
        indexes,q,exception);
      break;
    }
    case GrayQuantum:
    {
      ExportGrayQuantum(image,(QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case GrayAlphaQuantum:
    {
      ExportGrayAlphaQuantum(image,(QuantumInfo *) quantum_info,number_pixels,
        p,q);
      break;
    }
    case GreenQuantum:
    case MagentaQuantum:
    {
      ExportGreenQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case IndexQuantum:
    {
      ExportIndexQuantum(image,(QuantumInfo *) quantum_info,number_pixels,p,
        indexes,q,exception);
      break;
    }
    case IndexAlphaQuantum:
    {
      ExportIndexAlphaQuantum(image,(QuantumInfo *) quantum_info,number_pixels,
        p,indexes,q,exception);
      break;
    }
    case OpacityQuantum:
    {
      ExportOpacityQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case RedQuantum:
    case CyanQuantum:
    {
      ExportRedQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case RGBQuantum:
    case CbYCrQuantum:
    {
      ExportRGBQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case RGBAQuantum:
    case CbYCrAQuantum:
    {
      ExportRGBAQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    case RGBOQuantum:
    {
      ExportRGBOQuantum((QuantumInfo *) quantum_info,number_pixels,p,q);
      break;
    }
    default:
      break;
  }
  if ((quantum_type == CbYCrQuantum) || (quantum_type == CbYCrAQuantum))
    {
      Quantum
        quantum;

      register PixelPacket
        *magick_restrict q;

      q=GetAuthenticPixelQueue(image);
      if (image_view != (CacheView *) NULL)
        q=(PixelPacket *) GetCacheViewVirtualPixelQueue(image_view);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        quantum=GetPixelRed(q);
        SetPixelRed(q,GetPixelGreen(q));
        SetPixelGreen(q,quantum);
        q++;
      }
    }
  return(extent);
}