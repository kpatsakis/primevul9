static void XMakeImageLSBFirst(const XResourceInfo *resource_info,
  const XWindowInfo *window,Image *image,XImage *ximage,XImage *matte_image)
{
  CacheView
    *canvas_view;

  Image
    *canvas;

  int
    y;

  register const IndexPacket
    *indexes;

  register const PixelPacket
    *p;

  register int
    x;

  register unsigned char
    *q;

  unsigned char
    bit,
    byte;

  unsigned int
    scanline_pad;

  unsigned long
    pixel,
    *pixels;

  XStandardColormap
    *map_info;

  assert(resource_info != (XResourceInfo *) NULL);
  assert(window != (XWindowInfo *) NULL);
  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  canvas=image;
  if ((window->immutable == MagickFalse) &&
      (image->storage_class == DirectClass) && (image->matte != MagickFalse))
    {
      char
        size[MaxTextExtent];

      Image
        *pattern;

      ImageInfo
        *image_info;

      image_info=AcquireImageInfo();
      (void) CopyMagickString(image_info->filename,
        resource_info->image_info->texture != (char *) NULL ?
        resource_info->image_info->texture : "pattern:checkerboard",
        MaxTextExtent);
      (void) FormatLocaleString(size,MaxTextExtent,"%.20gx%.20g",(double)
        image->columns,(double) image->rows);
      image_info->size=ConstantString(size);
      pattern=ReadImage(image_info,&image->exception);
      image_info=DestroyImageInfo(image_info);
      if (pattern != (Image *) NULL)
        {
          canvas=CloneImage(image,0,0,MagickTrue,&image->exception);
          if (canvas != (Image *) NULL)
            (void) CompositeImage(canvas,DstOverCompositeOp,pattern,0,0);
          pattern=DestroyImage(pattern);
        }
    }
  scanline_pad=(unsigned int) (ximage->bytes_per_line-((ximage->width*
    ximage->bits_per_pixel) >> 3));
  map_info=window->map_info;
  pixels=window->pixel_info->pixels;
  q=(unsigned char *) ximage->data;
  x=0;
  canvas_view=AcquireVirtualCacheView(canvas,&canvas->exception);
  if (ximage->format == XYBitmap)
    {
      register unsigned short
        polarity;

      unsigned char
        background,
        foreground;

      /*
        Convert canvas to big-endian bitmap.
      */
      background=(unsigned char)
        (XPixelIntensity(&window->pixel_info->foreground_color) <
         XPixelIntensity(&window->pixel_info->background_color) ? 0x80 : 0x00);
      foreground=(unsigned char)
        (XPixelIntensity(&window->pixel_info->background_color) <
         XPixelIntensity(&window->pixel_info->foreground_color) ? 0x80 : 0x00);
      polarity=(unsigned short) ((GetPixelIntensity(image,
        &canvas->colormap[0])) < (QuantumRange/2.0) ? 1 : 0);
      if (canvas->colors == 2)
        polarity=GetPixelIntensity(canvas,&canvas->colormap[0]) <
          GetPixelIntensity(canvas,&canvas->colormap[1]);
      for (y=0; y < (int) canvas->rows; y++)
      {
        p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,canvas->columns,1,
          &canvas->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        indexes=GetCacheViewVirtualIndexQueue(canvas_view);
        bit=0;
        byte=0;
        for (x=0; x < (int) canvas->columns; x++)
        {
          byte>>=1;
          if (GetPixelIndex(indexes+x) == (IndexPacket) polarity)
            byte|=foreground;
          else
            byte|=background;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
        }
        if (bit != 0)
          *q=byte >> (8-bit);
        q+=scanline_pad;
      }
    }
  else
    if (window->pixel_info->colors != 0)
      switch (ximage->bits_per_pixel)
      {
        case 2:
        {
          register unsigned int
            nibble;

          /*
            Convert to 2 bit color-mapped X canvas.
          */
          for (y=0; y < (int) canvas->rows; y++)
          {
            p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,
              canvas->columns,1,&canvas->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetCacheViewVirtualIndexQueue(canvas_view);
            nibble=0;
            for (x=0; x < (int) canvas->columns; x++)
            {
              pixel=pixels[(ssize_t) GetPixelIndex(indexes+x)] & 0x0f;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 2);
                  nibble++;
                  break;
                }
                case 2:
                {
                  *q|=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 3:
                {
                  *q|=(unsigned char) (pixel << 6);
                  q++;
                  nibble=0;
                  break;
                }
              }
            }
            q+=scanline_pad;
          }
          break;
        }
        case 4:
        {
          register unsigned int
            nibble;

          /*
            Convert to 4 bit color-mapped X canvas.
          */
          for (y=0; y < (int) canvas->rows; y++)
          {
            p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,
              canvas->columns,1,&canvas->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetCacheViewVirtualIndexQueue(canvas_view);
            nibble=0;
            for (x=0; x < (int) canvas->columns; x++)
            {
              pixel=pixels[(ssize_t) GetPixelIndex(indexes+x)] & 0xf;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 4);
                  q++;
                  nibble=0;
                  break;
                }
              }
            }
            q+=scanline_pad;
          }
          break;
        }
        case 6:
        case 8:
        {
          /*
            Convert to 8 bit color-mapped X canvas.
          */
          if (resource_info->color_recovery &&
              resource_info->quantize_info->dither)
            {
              XDitherImage(canvas,ximage);
              break;
            }
          for (y=0; y < (int) canvas->rows; y++)
          {
            p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,
              canvas->columns,1,&canvas->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetCacheViewVirtualIndexQueue(canvas_view);
            for (x=0; x < (int) canvas->columns; x++)
            {
              pixel=pixels[(ssize_t) GetPixelIndex(indexes+x)];
              *q++=(unsigned char) pixel;
            }
            q+=scanline_pad;
          }
          break;
        }
        default:
        {
          register int
            k;

          register unsigned int
            bytes_per_pixel;

          /*
            Convert to multi-byte color-mapped X canvas.
          */
          bytes_per_pixel=(unsigned int) (ximage->bits_per_pixel >> 3);
          for (y=0; y < (int) canvas->rows; y++)
          {
            p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,
              canvas->columns,1,&canvas->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            indexes=GetCacheViewVirtualIndexQueue(canvas_view);
            for (x=0; x < (int) canvas->columns; x++)
            {
              pixel=pixels[(ssize_t) GetPixelIndex(indexes+x)];
              for (k=0; k < (int) bytes_per_pixel; k++)
              {
                *q++=(unsigned char) (pixel & 0xff);
                pixel>>=8;
              }
            }
            q+=scanline_pad;
          }
          break;
        }
      }
    else
      switch (ximage->bits_per_pixel)
      {
        case 2:
        {
          register unsigned int
            nibble;

          /*
            Convert to contiguous 2 bit continuous-tone X canvas.
          */
          for (y=0; y < (int) canvas->rows; y++)
          {
            nibble=0;
            p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,
              canvas->columns,1,&canvas->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            for (x=0; x < (int) canvas->columns; x++)
            {
              pixel=XGammaPixel(map_info,p);
              pixel&=0xf;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 2);
                  nibble++;
                  break;
                }
                case 2:
                {
                  *q|=(unsigned char) (pixel << 4);
                  nibble++;
                  break;
                }
                case 3:
                {
                  *q|=(unsigned char) (pixel << 6);
                  q++;
                  nibble=0;
                  break;
                }
              }
              p++;
            }
            q+=scanline_pad;
          }
          break;
        }
        case 4:
        {
          register unsigned int
            nibble;

          /*
            Convert to contiguous 4 bit continuous-tone X canvas.
          */
          for (y=0; y < (int) canvas->rows; y++)
          {
            p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,
              canvas->columns,1,&canvas->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            nibble=0;
            for (x=0; x < (int) canvas->columns; x++)
            {
              pixel=XGammaPixel(map_info,p);
              pixel&=0xf;
              switch (nibble)
              {
                case 0:
                {
                  *q=(unsigned char) pixel;
                  nibble++;
                  break;
                }
                case 1:
                {
                  *q|=(unsigned char) (pixel << 4);
                  q++;
                  nibble=0;
                  break;
                }
              }
              p++;
            }
            q+=scanline_pad;
          }
          break;
        }
        case 6:
        case 8:
        {
          /*
            Convert to contiguous 8 bit continuous-tone X canvas.
          */
          if (resource_info->color_recovery &&
              resource_info->quantize_info->dither)
            {
              XDitherImage(canvas,ximage);
              break;
            }
          for (y=0; y < (int) canvas->rows; y++)
          {
            p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,
              canvas->columns,1,&canvas->exception);
            if (p == (const PixelPacket *) NULL)
              break;
            for (x=0; x < (int) canvas->columns; x++)
            {
              pixel=XGammaPixel(map_info,p);
              *q++=(unsigned char) pixel;
              p++;
            }
            q+=scanline_pad;
          }
          break;
        }
        default:
        {
          if ((ximage->bits_per_pixel == 32) && (map_info->red_max == 255) &&
              (map_info->green_max == 255) && (map_info->blue_max == 255) &&
              (map_info->red_mult == 65536L) && (map_info->green_mult == 256) &&
              (map_info->blue_mult == 1))
            {
              /*
                Convert to 32 bit continuous-tone X canvas.
              */
              for (y=0; y < (int) canvas->rows; y++)
              {
                p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,
                  canvas->columns,1,&canvas->exception);
                if (p == (const PixelPacket *) NULL)
                  break;
                if ((red_gamma != 1.0) || (green_gamma != 1.0) ||
                    (blue_gamma != 1.0))
                  {
                    /*
                      Gamma correct canvas.
                    */
                    for (x=(int) canvas->columns-1; x >= 0; x--)
                    {
                      *q++=ScaleQuantumToChar(XBlueGamma(GetPixelBlue(p)));
                      *q++=ScaleQuantumToChar(XGreenGamma(GetPixelGreen(p)));
                      *q++=ScaleQuantumToChar(XRedGamma(GetPixelRed(p)));
                      *q++=0;
                      p++;
                    }
                    continue;
                  }
                for (x=(int) canvas->columns-1; x >= 0; x--)
                {
                  *q++=ScaleQuantumToChar((Quantum) GetPixelBlue(p));
                  *q++=ScaleQuantumToChar((Quantum) GetPixelGreen(p));
                  *q++=ScaleQuantumToChar((Quantum) GetPixelRed(p));
                  *q++=0;
                  p++;
                }
              }
            }
          else
            if ((ximage->bits_per_pixel == 32) && (map_info->red_max == 255) &&
                (map_info->green_max == 255) && (map_info->blue_max == 255) &&
                (map_info->red_mult == 1) && (map_info->green_mult == 256) &&
                (map_info->blue_mult == 65536L))
              {
                /*
                  Convert to 32 bit continuous-tone X canvas.
                */
                for (y=0; y < (int) canvas->rows; y++)
                {
                  p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,
                    canvas->columns,1,&canvas->exception);
                  if (p == (const PixelPacket *) NULL)
                    break;
                  if ((red_gamma != 1.0) || (green_gamma != 1.0) ||
                      (blue_gamma != 1.0))
                    {
                      /*
                        Gamma correct canvas.
                      */
                      for (x=(int) canvas->columns-1; x >= 0; x--)
                      {
                        *q++=ScaleQuantumToChar(XRedGamma(GetPixelRed(p)));
                        *q++=ScaleQuantumToChar(XGreenGamma(GetPixelGreen(p)));
                        *q++=ScaleQuantumToChar(XBlueGamma(GetPixelBlue(p)));
                        *q++=0;
                        p++;
                      }
                      continue;
                    }
                  for (x=(int) canvas->columns-1; x >= 0; x--)
                  {
                    *q++=ScaleQuantumToChar((Quantum) GetPixelRed(p));
                    *q++=ScaleQuantumToChar((Quantum) GetPixelGreen(p));
                    *q++=ScaleQuantumToChar((Quantum) GetPixelBlue(p));
                    *q++=0;
                    p++;
                  }
                }
              }
            else
              {
                register int
                  k;

                register unsigned int
                  bytes_per_pixel;

                /*
                  Convert to multi-byte continuous-tone X canvas.
                */
                bytes_per_pixel=(unsigned int) (ximage->bits_per_pixel >> 3);
                for (y=0; y < (int) canvas->rows; y++)
                {
                  p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,
                    canvas->columns,1,&canvas->exception);
                  if (p == (PixelPacket *) NULL)
                    break;
                  for (x=0; x < (int) canvas->columns; x++)
                  {
                    pixel=XGammaPixel(map_info,p);
                    for (k=0; k < (int) bytes_per_pixel; k++)
                    {
                      *q++=(unsigned char) (pixel & 0xff);
                      pixel>>=8;
                    }
                    p++;
                  }
                  q+=scanline_pad;
                }
              }
          break;
        }
      }
  if (matte_image != (XImage *) NULL)
    {
      /*
        Initialize matte canvas.
      */
      scanline_pad=(unsigned int) (matte_image->bytes_per_line-
        ((matte_image->width*matte_image->bits_per_pixel) >> 3));
      q=(unsigned char *) matte_image->data;
      for (y=0; y < (int) canvas->rows; y++)
      {
        p=GetCacheViewVirtualPixels(canvas_view,0,(ssize_t) y,canvas->columns,1,
          &canvas->exception);
        if (p == (const PixelPacket *) NULL)
          break;
        bit=0;
        byte=0;
        for (x=(int) canvas->columns-1; x >= 0; x--)
        {
          byte>>=1;
          if (GetPixelOpacity(p) > (QuantumRange/2))
            byte|=0x80;
          bit++;
          if (bit == 8)
            {
              *q++=byte;
              bit=0;
              byte=0;
            }
          p++;
        }
        if (bit != 0)
          *q=byte >> (8-bit);
        q+=scanline_pad;
      }
    }
  canvas_view=DestroyCacheView(canvas_view);
  if (canvas != image)
    canvas=DestroyImage(canvas);
}