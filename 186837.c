static Image *ReadGIFImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define BitSet(byte,bit)  (((byte) & (bit)) == (bit))
#define LSBFirstOrder(x,y)  (((y) << 8) | (x))

  Image
    *image,
    *meta_image;

  int
    number_extensionss=0;

  MagickBooleanType
    status;

  RectangleInfo
    page;

  register ssize_t
    i;

  register unsigned char
    *p;

  size_t
    delay,
    dispose,
    duration,
    global_colors,
    image_count,
    iterations,
    one;

  ssize_t
    count,
    opacity;

  unsigned char
    background,
    buffer[257],
    c,
    flag,
    *global_colormap;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Determine if this a GIF file.
  */
  count=ReadBlob(image,6,buffer);
  if ((count != 6) || ((LocaleNCompare((char *) buffer,"GIF87",5) != 0) &&
      (LocaleNCompare((char *) buffer,"GIF89",5) != 0)))
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  (void) ResetMagickMemory(buffer,0,sizeof(buffer));
  page.width=ReadBlobLSBShort(image);
  page.height=ReadBlobLSBShort(image);
  flag=(unsigned char) ReadBlobByte(image);
  background=(unsigned char) ReadBlobByte(image);
  c=(unsigned char) ReadBlobByte(image);  /* reserved */
  one=1;
  global_colors=one << (((size_t) flag & 0x07)+1);
  global_colormap=(unsigned char *) AcquireQuantumMemory((size_t)
    MagickMax(global_colors,256),3UL*sizeof(*global_colormap));
  if (global_colormap == (unsigned char *) NULL)
    ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
  (void) ResetMagickMemory(global_colormap,0,3*MagickMax(global_colors,256)*
    sizeof(*global_colormap));
  if (BitSet((int) flag,0x80) != 0)
    {
      count=ReadBlob(image,(size_t) (3*global_colors),global_colormap);
      if (count != (ssize_t) (3*global_colors))
        {
          global_colormap=(unsigned char *) RelinquishMagickMemory(
            global_colormap);
          ThrowReaderException(CorruptImageError,"InsufficientImageDataInFile");
        }
    }
  delay=0;
  dispose=0;
  duration=0;
  iterations=1;
  opacity=(-1);
  image_count=0;
  meta_image=AcquireImage(image_info,exception);  /* metadata container */
  for ( ; ; )
  {
    count=ReadBlob(image,1,&c);
    if (count != 1)
      break;
    if (c == (unsigned char) ';')
      break;  /* terminator */
    if (c == (unsigned char) '!')
      {
        /*
          GIF Extension block.
        */
        count=ReadBlob(image,1,&c);
        if (count != 1)
          {
            global_colormap=(unsigned char *) RelinquishMagickMemory(
              global_colormap);
            meta_image=DestroyImage(meta_image);
            ThrowReaderException(CorruptImageError,
              "UnableToReadExtensionBlock");
          }
        switch (c)
        {
          case 0xf9:
          {
            /*
              Read graphics control extension.
            */
            while (ReadBlobBlock(image,buffer) != 0) ;
            dispose=(size_t) buffer[0] >> 2;
            delay=((size_t) buffer[2] << 8) | buffer[1];
            if ((ssize_t) (buffer[0] & 0x01) == 0x01)
              opacity=(ssize_t) buffer[3];
            break;
          }
          case 0xfe:
          {
            char
              *comments;

            size_t
              length;

            /*
              Read comment extension.
            */
            comments=AcquireString((char *) NULL);
            for (length=0; ; length+=count)
            {
              count=(ssize_t) ReadBlobBlock(image,buffer);
              if (count == 0)
                break;
              buffer[count]='\0';
              (void) ConcatenateString(&comments,(const char *) buffer);
            }
            (void) SetImageProperty(meta_image,"comment",comments,exception);
            comments=DestroyString(comments);
            break;
          }
          case 0xff:
          {
            MagickBooleanType
              loop;

            /*
              Read Netscape Loop extension.
            */
            loop=MagickFalse;
            if (ReadBlobBlock(image,buffer) != 0)
              loop=LocaleNCompare((char *) buffer,"NETSCAPE2.0",11) == 0 ?
                MagickTrue : MagickFalse;
            if (loop != MagickFalse)
              while (ReadBlobBlock(image,buffer) != 0)
              {
                iterations=((size_t) buffer[2] << 8) | buffer[1];
                if (iterations != 0)
                  iterations++;
              }
            else
              {
                char
                  name[MagickPathExtent];

                int
                  block_length,
                  info_length,
                  reserved_length;

                MagickBooleanType
                  i8bim,
                  icc,
                  iptc,
                  magick;

                StringInfo
                  *profile;

                unsigned char
                  *info;

                /*
                  Store GIF application extension as a generic profile.
                */
                icc=LocaleNCompare((char *) buffer,"ICCRGBG1012",11) == 0 ?
                  MagickTrue : MagickFalse;
                magick=LocaleNCompare((char *) buffer,"ImageMagick",11) == 0 ?
                  MagickTrue : MagickFalse;
                i8bim=LocaleNCompare((char *) buffer,"MGK8BIM0000",11) == 0 ?
                  MagickTrue : MagickFalse;
                iptc=LocaleNCompare((char *) buffer,"MGKIPTC0000",11) == 0 ?
                  MagickTrue : MagickFalse;
                number_extensionss++;
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "    Reading GIF application extension");
                info=(unsigned char *) AcquireQuantumMemory(255UL,
                  sizeof(*info));
                if (info == (unsigned char *) NULL)
                  {
                    meta_image=DestroyImage(meta_image);
                    global_colormap=(unsigned char *) RelinquishMagickMemory(
                      global_colormap);
                    ThrowReaderException(ResourceLimitError,
                      "MemoryAllocationFailed");
                  }
                reserved_length=255;
                for (info_length=0; ; )
                {
                  block_length=(int) ReadBlobBlock(image,&info[info_length]);
                  if (block_length == 0)
                    break;
                  info_length+=block_length;
                  if (info_length > (reserved_length-255))
                    {
                      reserved_length+=4096;
                      info=(unsigned char *) ResizeQuantumMemory(info,(size_t)
                        reserved_length,sizeof(*info));
                      if (info == (unsigned char *) NULL)
                        {
                          meta_image=DestroyImage(meta_image);
                          global_colormap=(unsigned char *)
                            RelinquishMagickMemory(global_colormap);
                          ThrowReaderException(ResourceLimitError,
                            "MemoryAllocationFailed");
                        }
                    }
                }
                profile=BlobToStringInfo(info,(size_t) info_length);
                if (profile == (StringInfo *) NULL)
                  {
                    meta_image=DestroyImage(meta_image);
                    global_colormap=(unsigned char *) RelinquishMagickMemory(
                      global_colormap);
                    info=(unsigned char *) RelinquishMagickMemory(info);
                    ThrowReaderException(ResourceLimitError,
                      "MemoryAllocationFailed");
                  }
                if (i8bim != MagickFalse)
                  (void) CopyMagickString(name,"8bim",sizeof(name));
                else if (icc != MagickFalse)
                  (void) CopyMagickString(name,"icc",sizeof(name));
                else if (iptc != MagickFalse)
                  (void) CopyMagickString(name,"iptc",sizeof(name));
                else if (magick != MagickFalse)
                  {
                    (void) CopyMagickString(name,"magick",sizeof(name));
                    meta_image->gamma=StringToDouble((char *) info+6,
                      (char **) NULL);
                  }
                else
                  (void) FormatLocaleString(name,sizeof(name),"gif:%.11s",
                    buffer);
                info=(unsigned char *) RelinquishMagickMemory(info);
                if (magick == MagickFalse)
                  (void) SetImageProfile(meta_image,name,profile,exception);
                profile=DestroyStringInfo(profile);
                (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                  "      profile name=%s",name);
              }
            break;
          }
          default:
          {
            while (ReadBlobBlock(image,buffer) != 0) ;
            break;
          }
        }
      }
    if (c != (unsigned char) ',')
      continue;
    if (image_count != 0)
      {
        /*
          Allocate next image structure.
        */
        AcquireNextImage(image_info,image,exception);
        if (GetNextImageInList(image) == (Image *) NULL)
          {
            image=DestroyImageList(image);
            global_colormap=(unsigned char *) RelinquishMagickMemory(
              global_colormap);
            return((Image *) NULL);
          }
        image=SyncNextImageInList(image);
      }
    image_count++;
    /*
      Read image attributes.
    */
    meta_image->scene=image->scene;
    (void) CloneImageProperties(image,meta_image);
    DestroyImageProperties(meta_image);
    (void) CloneImageProfiles(image,meta_image);
    DestroyImageProfiles(meta_image);
    image->storage_class=PseudoClass;
    image->compression=LZWCompression;
    page.x=(ssize_t) ReadBlobLSBShort(image);
    page.y=(ssize_t) ReadBlobLSBShort(image);
    image->columns=ReadBlobLSBShort(image);
    image->rows=ReadBlobLSBShort(image);
    image->depth=8;
    flag=(unsigned char) ReadBlobByte(image);
    image->interlace=BitSet((int) flag,0x40) != 0 ? GIFInterlace : NoInterlace;
    image->colors=BitSet((int) flag,0x80) == 0 ? global_colors : one <<
      ((size_t) (flag & 0x07)+1);
    if (opacity >= (ssize_t) image->colors)
      image->colors=opacity+1;
    image->page.width=page.width;
    image->page.height=page.height;
    image->page.y=page.y;
    image->page.x=page.x;
    image->delay=delay;
    image->ticks_per_second=100;
    image->dispose=(DisposeType) dispose;
    image->iterations=iterations;
    image->alpha_trait=opacity >= 0 ? BlendPixelTrait : UndefinedPixelTrait;
    delay=0;
    dispose=0;
    if ((image->columns == 0) || (image->rows == 0))
      {
        global_colormap=(unsigned char *) RelinquishMagickMemory(
          global_colormap);
        meta_image=DestroyImage(meta_image);
        ThrowReaderException(CorruptImageError,"NegativeOrZeroImageSize");
      }
    /*
      Inititialize colormap.
    */
    if (AcquireImageColormap(image,image->colors,exception) == MagickFalse)
      {
        global_colormap=(unsigned char *) RelinquishMagickMemory(
          global_colormap);
        meta_image=DestroyImage(meta_image);
        ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
      }
    if (BitSet((int) flag,0x80) == 0)
      {
        /*
          Use global colormap.
        */
        p=global_colormap;
        for (i=0; i < (ssize_t) image->colors; i++)
        {
          image->colormap[i].red=(double) ScaleCharToQuantum(*p++);
          image->colormap[i].green=(double) ScaleCharToQuantum(*p++);
          image->colormap[i].blue=(double) ScaleCharToQuantum(*p++);
          if (i == opacity)
            {
              image->colormap[i].alpha=(double) TransparentAlpha;
              image->transparent_color=image->colormap[opacity];
            }
        }
        image->background_color=image->colormap[MagickMin((ssize_t) background,
          (ssize_t) image->colors-1)];
      }
    else
      {
        unsigned char
          *colormap;

        /*
          Read local colormap.
        */
        colormap=(unsigned char *) AcquireQuantumMemory(image->colors,3*
          sizeof(*colormap));
        if (colormap == (unsigned char *) NULL)
          {
            global_colormap=(unsigned char *) RelinquishMagickMemory(
              global_colormap);
            meta_image=DestroyImage(meta_image);
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          }
        count=ReadBlob(image,(3*image->colors)*sizeof(*colormap),colormap);
        if (count != (ssize_t) (3*image->colors))
          {
            global_colormap=(unsigned char *) RelinquishMagickMemory(
              global_colormap);
            colormap=(unsigned char *) RelinquishMagickMemory(colormap);
            meta_image=DestroyImage(meta_image);
            ThrowReaderException(CorruptImageError,
              "InsufficientImageDataInFile");
          }
        p=colormap;
        for (i=0; i < (ssize_t) image->colors; i++)
        {
          image->colormap[i].red=(double) ScaleCharToQuantum(*p++);
          image->colormap[i].green=(double) ScaleCharToQuantum(*p++);
          image->colormap[i].blue=(double) ScaleCharToQuantum(*p++);
          if (i == opacity)
            image->colormap[i].alpha=(double) TransparentAlpha;
        }
        colormap=(unsigned char *) RelinquishMagickMemory(colormap);
      }
    if (image->gamma == 1.0)
      {
        for (i=0; i < (ssize_t) image->colors; i++)
          if (IsPixelInfoGray(image->colormap+i) == MagickFalse)
            break;
        (void) SetImageColorspace(image,i == (ssize_t) image->colors ? 
          GRAYColorspace : RGBColorspace,exception);
      }
    if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    status=SetImageExtent(image,image->columns,image->rows,exception);
    if (status == MagickFalse)
      {
        global_colormap=(unsigned char *) RelinquishMagickMemory(
          global_colormap);
        meta_image=DestroyImage(meta_image);
        return(DestroyImageList(image));
      }
    /*
      Decode image.
    */
    if (image_info->ping != MagickFalse)
      status=PingGIFImage(image,exception);
    else
      status=DecodeImage(image,opacity,exception);
    if ((image_info->ping == MagickFalse) && (status == MagickFalse))
      {
        global_colormap=(unsigned char *) RelinquishMagickMemory(
          global_colormap);
        meta_image=DestroyImage(meta_image);
        ThrowReaderException(CorruptImageError,"CorruptImage");
      }
    duration+=image->delay*image->iterations;
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    opacity=(-1);
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) image->scene-
      1,image->scene);
    if (status == MagickFalse)
      break;
  }
  image->duration=duration;
  meta_image=DestroyImage(meta_image);
  global_colormap=(unsigned char *) RelinquishMagickMemory(global_colormap);
  if ((image->columns == 0) || (image->rows == 0))
    ThrowReaderException(CorruptImageError,"NegativeOrZeroImageSize");
  (void) CloseBlob(image);
  return(GetFirstImageInList(image));
}