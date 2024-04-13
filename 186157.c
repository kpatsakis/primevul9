static MagickBooleanType WritePS3Image(const ImageInfo *image_info,Image *image)
{
  static const char
    *const PostscriptProlog[]=
    {
      "/ByteStreamDecodeFilter",
      "{",
      "  /z exch def",
      "  /r exch def",
      "  /c exch def",
      "  z " PS3_NoCompression " eq { /ASCII85Decode filter } if",
      "  z " PS3_FaxCompression " eq",
      "  {",
      "    <<",
      "      /K " CCITTParam,
      "      /Columns c",
      "      /Rows r",
      "    >>",
      "    /CCITTFaxDecode filter",
      "  } if",
      "  z " PS3_JPEGCompression " eq { /DCTDecode filter } if",
      "  z " PS3_LZWCompression " eq { /LZWDecode filter } if",
      "  z " PS3_RLECompression " eq { /RunLengthDecode filter } if",
      "  z " PS3_ZipCompression " eq { /FlateDecode filter } if",
      "} bind def",
      "",
      "/DirectClassImageDict",
      "{",
      "  colorspace " PS3_RGBColorspace " eq",
      "  {",
      "    /DeviceRGB setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /DataSource pixel_stream",
      "      /MultipleDataSources false",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      /Decode [0 1 0 1 0 1]",
      "    >>",
      "  }",
      "  {",
      "    /DeviceCMYK setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /DataSource pixel_stream",
      "      /MultipleDataSources false",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      /Decode",
      "        compression " PS3_JPEGCompression " eq",
      "        { [1 0 1 0 1 0 1 0] }",
      "        { [0 1 0 1 0 1 0 1] }",
      "        ifelse",
      "    >>",
      "  }",
      "  ifelse",
      "} bind def",
      "",
      "/PseudoClassImageDict",
      "{",
      "  % Colors in colormap image.",
      "  currentfile buffer readline pop",
      "  token pop /colors exch def pop",
      "  colors 0 eq",
      "  {",
      "    % Depth of grayscale image.",
      "    currentfile buffer readline pop",
      "    token pop /bits exch def pop",
      "    /DeviceGray setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent bits",
      "      /Decode [0 1]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      /DataSource pixel_stream",
      "    >>",
      "  }",
      "  {",
      "    % RGB colormap.",
      "    /colormap colors 3 mul string def",
      "    compression " PS3_NoCompression " eq",
      "    { currentfile /ASCII85Decode filter colormap readstring pop pop }",
      "    { currentfile colormap readstring pop pop }",
      "    ifelse",
      "    [ /Indexed /DeviceRGB colors 1 sub colormap ] setcolorspace",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 8",
      "      /Decode [0 255]",
      "      /ImageMatrix [columns 0 0 rows neg 0 rows]",
      "      /DataSource pixel_stream",
      "    >>",
      "  }",
      "  ifelse",
      "} bind def",
      "",
      "/NonMaskedImageDict",
      "{",
      "  class " PS3_PseudoClass " eq",
      "  { PseudoClassImageDict }",
      "  { DirectClassImageDict }",
      "  ifelse",
      "} bind def",
      "",
      "/MaskedImageDict",
      "{",
      "  <<",
      "    /ImageType 3",
      "    /InterleaveType 3",
      "    /DataDict NonMaskedImageDict",
      "    /MaskDict",
      "    <<",
      "      /ImageType 1",
      "      /Width columns",
      "      /Height rows",
      "      /BitsPerComponent 1",
      "      /DataSource mask_stream",
      "      /MultipleDataSources false",
      "      /ImageMatrix [ columns 0 0 rows neg 0 rows]",
      "      /Decode [ 0 1 ]",
      "    >>",
      "  >>",
      "} bind def",
      "",
      "/ClipImage",
      "{} def",
      "",
      "/DisplayImage",
      "{",
      "  gsave",
      "  /buffer 512 string def",
      "  % Translation.",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  x y translate",
      "  % Image size and font size.",
      "  currentfile buffer readline pop",
      "  token pop /x exch def",
      "  token pop /y exch def pop",
      "  currentfile buffer readline pop",
      "  token pop /pointsize exch def pop",
      (const char *) NULL
    },
    *const PostscriptEpilog[]=
    {
      "  x y scale",
      "  % Clipping path.",
      "  currentfile buffer readline pop",
      "  token pop /clipped exch def pop",
      "  % Showpage.",
      "  currentfile buffer readline pop",
      "  token pop /sp exch def pop",
      "  % Image pixel size.",
      "  currentfile buffer readline pop",
      "  token pop /columns exch def",
      "  token pop /rows exch def pop",
      "  % Colorspace (RGB/CMYK).",
      "  currentfile buffer readline pop",
      "  token pop /colorspace exch def pop",
      "  % Transparency.",
      "  currentfile buffer readline pop",
      "  token pop /alpha exch def pop",
      "  % Stencil mask?",
      "  currentfile buffer readline pop",
      "  token pop /stencil exch def pop",
      "  % Image class (direct/pseudo).",
      "  currentfile buffer readline pop",
      "  token pop /class exch def pop",
      "  % Compression type.",
      "  currentfile buffer readline pop",
      "  token pop /compression exch def pop",
      "  % Clip and render.",
      "  /pixel_stream currentfile columns rows compression ByteStreamDecodeFilter def",
      "  clipped { ClipImage } if",
      "  alpha stencil not and",
      "  { MaskedImageDict mask_stream resetfile }",
      "  { NonMaskedImageDict }",
      "  ifelse",
      "  stencil { 0 setgray imagemask } { image } ifelse",
      "  grestore",
      "  sp { showpage } if",
      "} bind def",
      (const char *) NULL
    };

  char
    buffer[MaxTextExtent],
    date[MaxTextExtent],
    **labels,
    page_geometry[MaxTextExtent];

  CompressionType
    compression;

  const char
    *const *q,
    *option,
    *value;

  double
    pointsize;

  GeometryInfo
    geometry_info;

  MagickBooleanType
    status;

  MagickOffsetType
    offset,
    scene,
    start,
    stop;

  MagickStatusType
    flags;

  MemoryInfo
    *pixel_info;

  PointInfo
    delta,
    resolution,
    scale;

  RectangleInfo
    geometry,
    media_info,
    page_info;

  register ssize_t
    i;

  SegmentInfo
    bounds;

  size_t
    imageListLength,
    length,
    page,
    pixel,
    text_size;

  ssize_t
    j;

  time_t
    timer;

  unsigned char
    *pixels;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,&image->exception);
  if (status == MagickFalse)
    return(MagickFalse);
  compression=image->compression;
  if (image_info->compression != UndefinedCompression)
    compression=image_info->compression;
  switch (compression)
  {
    case FaxCompression:
    case Group4Compression:
    {
      if ((SetImageMonochrome(image,&image->exception) == MagickFalse) ||
          (image->matte != MagickFalse))
        compression=RLECompression;
      break;
    }
#if !defined(MAGICKCORE_JPEG_DELEGATE)
    case JPEGCompression:
    {
      compression=RLECompression;
      (void) ThrowMagickException(&image->exception,GetMagickModule(),
        MissingDelegateError,"DelegateLibrarySupportNotBuiltIn","`%s' (JPEG)",
        image->filename);
      break;
    }
#endif
#if !defined(MAGICKCORE_ZLIB_DELEGATE)
    case ZipCompression:
    {
      compression=RLECompression;
      (void) ThrowMagickException(&image->exception,GetMagickModule(),
        MissingDelegateError,"DelegateLibrarySupportNotBuiltIn","`%s' (ZLIB)",
        image->filename);
      break;
    }
#endif
    default:
      break;
  }
  (void) memset(&bounds,0,sizeof(bounds));
  page=0;
  scene=0;
  imageListLength=GetImageListLength(image);
  do
  {
    /*
      Scale relative to dots-per-inch.
    */
    delta.x=DefaultResolution;
    delta.y=DefaultResolution;
    resolution.x=image->x_resolution;
    resolution.y=image->y_resolution;
    if ((resolution.x == 0.0) || (resolution.y == 0.0))
      {
        flags=ParseGeometry(PSDensityGeometry,&geometry_info);
        resolution.x=geometry_info.rho;
        resolution.y=geometry_info.sigma;
        if ((flags & SigmaValue) == 0)
          resolution.y=resolution.x;
      }
    if (image_info->density != (char *) NULL)
      {
        flags=ParseGeometry(image_info->density,&geometry_info);
        resolution.x=geometry_info.rho;
        resolution.y=geometry_info.sigma;
        if ((flags & SigmaValue) == 0)
          resolution.y=resolution.x;
      }
    if (image->units == PixelsPerCentimeterResolution)
      {
        resolution.x=(100.0*2.54*resolution.x+0.5)/100.0;
        resolution.y=(100.0*2.54*resolution.y+0.5)/100.0;
      }
    SetGeometry(image,&geometry);
    (void) FormatLocaleString(page_geometry,MaxTextExtent,"%.20gx%.20g",
      (double) image->columns,(double) image->rows);
    if (image_info->page != (char *) NULL)
      (void) CopyMagickString(page_geometry,image_info->page,MaxTextExtent);
    else
      if ((image->page.width != 0) && (image->page.height != 0))
        (void) FormatLocaleString(page_geometry,MaxTextExtent,
          "%.20gx%.20g%+.20g%+.20g",(double) image->page.width,(double)
          image->page.height,(double) image->page.x,(double) image->page.y);
      else
        if ((image->gravity != UndefinedGravity) &&
            (LocaleCompare(image_info->magick,"PS") == 0))
          (void) CopyMagickString(page_geometry,PSPageGeometry,MaxTextExtent);
    (void) ConcatenateMagickString(page_geometry,">",MaxTextExtent);
    (void) ParseMetaGeometry(page_geometry,&geometry.x,&geometry.y,
      &geometry.width,&geometry.height);
    scale.x=PerceptibleReciprocal(resolution.x)*geometry.width*delta.x;
    geometry.width=(size_t) floor(scale.x+0.5);
    scale.y=PerceptibleReciprocal(resolution.y)*geometry.height*delta.y;
    geometry.height=(size_t) floor(scale.y+0.5);
    (void) ParseAbsoluteGeometry(page_geometry,&media_info);
    (void) ParseGravityGeometry(image,page_geometry,&page_info,
      &image->exception);
    if (image->gravity != UndefinedGravity)
      {
        geometry.x=(-page_info.x);
        geometry.y=(ssize_t) (media_info.height+page_info.y-image->rows);
      }
    pointsize=12.0;
    if (image_info->pointsize != 0.0)
      pointsize=image_info->pointsize;
    text_size=0;
    value=GetImageProperty(image,"label");
    if (value != (const char *) NULL)
      text_size=(size_t) (MultilineCensus(value)*pointsize+12);
    page++;
    if (page == 1)
      {
        /*
          Postscript header on the first page.
        */
        if (LocaleCompare(image_info->magick,"PS3") == 0)
          (void) CopyMagickString(buffer,"%!PS-Adobe-3.0\n",MaxTextExtent);
        else
          (void) CopyMagickString(buffer,"%!PS-Adobe-3.0 EPSF-3.0\n",
            MaxTextExtent);
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MaxTextExtent,
          "%%%%Creator: ImageMagick %s\n",MagickLibVersionText);
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MaxTextExtent,"%%%%Title: %s\n",
          image->filename);
        (void) WriteBlobString(image,buffer);
        timer=GetMagickTime();
        (void) FormatMagickTime(timer,MaxTextExtent,date);
        (void) FormatLocaleString(buffer,MaxTextExtent,
          "%%%%CreationDate: %s\n",date);
        (void) WriteBlobString(image,buffer);
        bounds.x1=(double) geometry.x;
        bounds.y1=(double) geometry.y;
        bounds.x2=(double) geometry.x+scale.x;
        bounds.y2=(double) geometry.y+scale.y+text_size;
        if ((image_info->adjoin != MagickFalse) &&
            (GetNextImageInList(image) != (Image *) NULL))
          {
            (void) WriteBlobString(image,"%%BoundingBox: (atend)\n");
            (void) WriteBlobString(image,"%%HiResBoundingBox: (atend)\n");
          }
        else
          {
            (void) FormatLocaleString(buffer,MaxTextExtent,
              "%%%%BoundingBox: %g %g %g %g\n",ceil(bounds.x1-0.5),
              ceil(bounds.y1-0.5),floor(bounds.x2+0.5),floor(bounds.y2+0.5));
            (void) WriteBlobString(image,buffer);
            (void) FormatLocaleString(buffer,MaxTextExtent,
              "%%%%HiResBoundingBox: %g %g %g %g\n",bounds.x1,
              bounds.y1,bounds.x2,bounds.y2);
            (void) WriteBlobString(image,buffer);
            if (image->colorspace == CMYKColorspace)
              (void) WriteBlobString(image,
                "%%DocumentProcessColors: Cyan Magenta Yellow Black\n");
            else
              if (SetImageGray(image,&image->exception) != MagickFalse)
                (void) WriteBlobString(image,
                  "%%DocumentProcessColors: Black\n");
          }
        /*
          Font resources
        */
        value=GetImageProperty(image,"label");
        if (value != (const char *) NULL)
          (void) WriteBlobString(image,
            "%%DocumentNeededResources: font Helvetica\n");
        (void) WriteBlobString(image,"%%LanguageLevel: 3\n");
        /*
          Pages, orientation and order.
        */
        if (LocaleCompare(image_info->magick,"PS3") != 0)
          (void) WriteBlobString(image,"%%Pages: 1\n");
        else
          {
            (void) WriteBlobString(image,"%%Orientation: Portrait\n");
            (void) WriteBlobString(image,"%%PageOrder: Ascend\n");
            if (image_info->adjoin == MagickFalse)
              (void) CopyMagickString(buffer,"%%Pages: 1\n",MaxTextExtent);
            else
              (void) FormatLocaleString(buffer,MaxTextExtent,
                "%%%%Pages: %.20g\n",(double) GetImageListLength(image));
            (void) WriteBlobString(image,buffer);
          }
        if (image->colorspace == CMYKColorspace)
          (void) WriteBlobString(image,
            "%%DocumentProcessColors: Cyan Magenta Yellow Black\n");
        (void) WriteBlobString(image,"%%EndComments\n");
        /*
          The static postscript procedures prolog.
        */
        (void)WriteBlobString(image,"%%BeginProlog\n");
        for (q=PostscriptProlog; *q; q++)
        {
          (void) WriteBlobString(image,*q);
          (void) WriteBlobByte(image,'\n');
        }
        /*
          One label line for each line in label string.
        */
        value=GetImageProperty(image,"label");
        if (value != (const char *) NULL)
          {
              (void) WriteBlobString(image,"\n  %% Labels.\n  /Helvetica "
              " findfont pointsize scalefont setfont\n");
            for (i=(ssize_t) MultilineCensus(value)-1; i >= 0; i--)
            {
              (void) WriteBlobString(image,
                "  currentfile buffer readline pop token pop\n");
              (void) FormatLocaleString(buffer,MaxTextExtent,
                "  0 y %g add moveto show pop\n",i*pointsize+12);
              (void) WriteBlobString(image,buffer);
            }
          }
        /*
          The static postscript procedures epilog.
        */
        for (q=PostscriptEpilog; *q; q++)
        {
          (void) WriteBlobString(image,*q);
          (void) WriteBlobByte(image,'\n');
        }
        (void)WriteBlobString(image,"%%EndProlog\n");
      }
    (void) FormatLocaleString(buffer,MaxTextExtent,"%%%%Page: 1 %.20g\n",
      (double) page);
    (void) WriteBlobString(image,buffer);
    /*
      Page bounding box.
    */
    (void) FormatLocaleString(buffer,MaxTextExtent,
      "%%%%PageBoundingBox: %.20g %.20g %.20g %.20g\n",(double) geometry.x,
       (double) geometry.y,geometry.x+(double) geometry.width,geometry.y+
       (double) (geometry.height+text_size));
    (void) WriteBlobString(image,buffer);
    /*
      Page process colors if not RGB.
    */
    if (image->colorspace == CMYKColorspace)
      (void) WriteBlobString(image,
        "%%PageProcessColors: Cyan Magenta Yellow Black\n");
    else
      if (SetImageGray(image,&image->exception) != MagickFalse)
        (void) WriteBlobString(image,"%%PageProcessColors: Black\n");
    /*
      Adjust document bounding box to bound page bounding box.
    */
    if ((double) geometry.x < bounds.x1)
      bounds.x1=(double) geometry.x;
    if ((double) geometry.y < bounds.y1)
      bounds.y1=(double) geometry.y;
    if ((double) (geometry.x+scale.x) > bounds.x2)
      bounds.x2=(double) geometry.x+scale.x;
    if ((double) (geometry.y+scale.y+text_size) > bounds.y2)
      bounds.y2=(double) geometry.y+scale.y+text_size;
    /*
      Page font resource if there's a label.
    */
    value=GetImageProperty(image,"label");
    if (value != (const char *) NULL)
      (void) WriteBlobString(image,"%%PageResources: font Helvetica\n");
    /*
      PS clipping path from Photoshop clipping path.
    */
    if ((image->clip_mask == (Image *) NULL) ||
        (LocaleNCompare("8BIM:",image->clip_mask->magick_filename,5) != 0))
      (void) WriteBlobString(image,"/ClipImage {} def\n");
    else
      {
        const char
          *value;

        value=GetImageProperty(image,image->clip_mask->magick_filename);
        if (value == (const char *) NULL)
          return(MagickFalse);
        (void) WriteBlobString(image,value);
        (void) WriteBlobByte(image,'\n');
      }
    /*
      Push a dictionary for our own def's if this an EPS.
    */
    if (LocaleCompare(image_info->magick,"PS3") != 0)
      (void) WriteBlobString(image,"userdict begin\n");
    /*
      Image mask.
    */
    if ((image->matte != MagickFalse) &&
        (WritePS3MaskImage(image_info,image,compression) == MagickFalse))
      {
        (void) CloseBlob(image);
        return(MagickFalse);
      }
    /*
      Remember position of BeginData comment so we can update it.
    */
    start=TellBlob(image);
    if (start < 0)
      ThrowWriterException(CorruptImageError,"ImproperImageHeader");
    (void) FormatLocaleString(buffer,MaxTextExtent,
      "%%%%BeginData:%13ld %s Bytes\n",0L,
      compression == NoCompression ? "ASCII" : "BINARY");
    (void) WriteBlobString(image,buffer);
    stop=TellBlob(image);
    if (stop < 0)
      ThrowWriterException(CorruptImageError,"ImproperImageHeader");
    (void) WriteBlobString(image,"DisplayImage\n");
    /*
      Translate, scale, and font point size.
    */
    (void) FormatLocaleString(buffer,MaxTextExtent,"%.20g %.20g\n%g %g\n%g\n",
      (double) geometry.x,(double) geometry.y,scale.x,scale.y,pointsize);
    (void) WriteBlobString(image,buffer);
    /*
      Output labels.
    */
    labels=(char **) NULL;
    value=GetImageProperty(image,"label");
    if (value != (const char *) NULL)
      labels=StringToList(value);
    if (labels != (char **) NULL)
      {
        for (i=0; labels[i] != (char *) NULL; i++)
        {
          if (compression != NoCompression)
            {
              for (j=0; labels[i][j] != '\0'; j++)
                (void) WriteBlobByte(image,(unsigned char) labels[i][j]);
              (void) WriteBlobByte(image,'\n');
            }
          else
            {
              (void) WriteBlobString(image,"<~");
              Ascii85Initialize(image);
              for (j=0; labels[i][j] != '\0'; j++)
                Ascii85Encode(image,(unsigned char) labels[i][j]);
              Ascii85Flush(image);
            }
          labels[i]=DestroyString(labels[i]);
        }
        labels=(char **) RelinquishMagickMemory(labels);
      }
    /*
      Photoshop clipping path active?
    */
    if ((image->clip_mask != (Image *) NULL) &&
        (LocaleNCompare("8BIM:",image->clip_mask->magick_filename,5) == 0))
        (void) WriteBlobString(image,"true\n");
      else
        (void) WriteBlobString(image,"false\n");
    /*
      Showpage for non-EPS.
    */
    (void) WriteBlobString(image, LocaleCompare(image_info->magick,"PS3") == 0 ?
      "true\n" : "false\n");
    /*
      Image columns, rows, and color space.
    */
    (void) FormatLocaleString(buffer,MaxTextExtent,"%.20g %.20g\n%s\n",
      (double) image->columns,(double) image->rows,image->colorspace ==
      CMYKColorspace ? PS3_CMYKColorspace : PS3_RGBColorspace);
    (void) WriteBlobString(image,buffer);
    /*
      Masked image?
    */
    (void) WriteBlobString(image,image->matte != MagickFalse ?
      "true\n" : "false\n");
    /*
      Render with imagemask operator?
    */
    option=GetImageOption(image_info,"ps3:imagemask");
    (void) WriteBlobString(image,((option != (const char *) NULL) &&
      (SetImageMonochrome(image,&image->exception) != MagickFalse)) ?
      "true\n" : "false\n");
    /*
      Output pixel data.
    */
    pixels=(unsigned char *) NULL;
    length=0;
    if ((image_info->type != TrueColorType) &&
        (image_info->type != TrueColorMatteType) &&
        (image_info->type != ColorSeparationType) &&
        (image_info->type != ColorSeparationMatteType) &&
        (image->colorspace != CMYKColorspace) &&
        ((SetImageGray(image,&image->exception) != MagickFalse) ||
         (SetImageMonochrome(image,&image->exception) != MagickFalse)))
      {
        /*
          Gray images.
        */
        (void) WriteBlobString(image,PS3_PseudoClass"\n");
        switch (compression)
        {
          case NoCompression:
          default:
          {
            (void) WriteBlobString(image,PS3_NoCompression"\n");
            break;
          }
          case FaxCompression:
          case Group4Compression:
          {
            (void) WriteBlobString(image,PS3_FaxCompression"\n");
            break;
          }
          case JPEGCompression:
          {
            (void) WriteBlobString(image,PS3_JPEGCompression"\n");
            break;
          }
          case LZWCompression:
          {
            (void) WriteBlobString(image,PS3_LZWCompression"\n");
            break;
          }
          case RLECompression:
          {
            (void) WriteBlobString(image,PS3_RLECompression"\n");
            break;
          }
          case ZipCompression:
          {
            (void) WriteBlobString(image,PS3_ZipCompression"\n");
            break;
          }
        }
        /*
          Number of colors -- 0 for single component non-color mapped data.
        */
        (void) WriteBlobString(image,"0\n");
        /*
          1 bit or 8 bit components?
        */
        (void) FormatLocaleString(buffer,MaxTextExtent,"%d\n",
          SetImageMonochrome(image,&image->exception) != MagickFalse ? 1 : 8);
        (void) WriteBlobString(image,buffer);
        /*
          Image data.
        */
        if (compression == JPEGCompression)
          status=InjectImageBlob(image_info,image,image,"jpeg",
            &image->exception);
        else
          if ((compression == FaxCompression) ||
              (compression == Group4Compression))
            {
              if (LocaleCompare(CCITTParam,"0") == 0)
                status=HuffmanEncodeImage(image_info,image,image);
              else
                status=Huffman2DEncodeImage(image_info,image,image);
            }
          else
            {
              status=SerializeImageChannel(image_info,image,&pixel_info,
                &length);
              if (status == MagickFalse)
                {
                  (void) CloseBlob(image);
                  return(MagickFalse);
                }
              pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
              switch (compression)
              {
                case NoCompression:
                default:
                {
                  Ascii85Initialize(image);
                  for (i=0; i < (ssize_t) length; i++)
                    Ascii85Encode(image,pixels[i]);
                  Ascii85Flush(image);
                  status=MagickTrue;
                  break;
                }
                case LZWCompression:
                {
                  status=LZWEncodeImage(image,length,pixels);
                  break;
                }
                case RLECompression:
                {
                  status=PackbitsEncodeImage(image,length,pixels);
                  break;
                }
                case ZipCompression:
                {
                  status=ZLIBEncodeImage(image,length,pixels);
                  break;
                }
              }
              pixel_info=RelinquishVirtualMemory(pixel_info);
            }
      }
    else
      if ((image->storage_class == DirectClass) || (image->colors > 256) ||
          (compression == JPEGCompression))
        {
          /*
            Truecolor image.
          */
          (void) WriteBlobString(image,PS3_DirectClass"\n");
          switch (compression)
          {
            case NoCompression:
            default:
            {
              (void) WriteBlobString(image,PS3_NoCompression"\n");
              break;
            }
            case RLECompression:
            {
              (void) WriteBlobString(image,PS3_RLECompression"\n");
              break;
            }
            case JPEGCompression:
            {
              (void) WriteBlobString(image,PS3_JPEGCompression"\n");
              break;
            }
            case LZWCompression:
            {
              (void) WriteBlobString(image,PS3_LZWCompression"\n");
              break;
            }
            case ZipCompression:
            {
              (void) WriteBlobString(image,PS3_ZipCompression"\n");
              break;
            }
          }
          /*
            Image data.
          */
          if (compression == JPEGCompression)
            status=InjectImageBlob(image_info,image,image,"jpeg",
              &image->exception);
          else
            {
              /*
                Stream based compressions.
              */
              status=SerializeImage(image_info,image,&pixel_info,&length);
              if (status == MagickFalse)
                {
                  (void) CloseBlob(image);
                  return(MagickFalse);
                }
              pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
              switch (compression)
              {
                case NoCompression:
                default:
                {
                  Ascii85Initialize(image);
                  for (i=0; i < (ssize_t) length; i++)
                    Ascii85Encode(image,pixels[i]);
                  Ascii85Flush(image);
                  status=MagickTrue;
                  break;
                }
                case RLECompression:
                {
                  status=PackbitsEncodeImage(image,length,pixels);
                  break;
                }
                case LZWCompression:
                {
                  status=LZWEncodeImage(image,length,pixels);
                  break;
                }
                case ZipCompression:
                {
                  status=ZLIBEncodeImage(image,length,pixels);
                  break;
                }
              }
              pixel_info=RelinquishVirtualMemory(pixel_info);
            }
          }
        else
          {
            /*
              Colormapped images.
            */
            (void) WriteBlobString(image,PS3_PseudoClass"\n");
            switch (compression)
            {
              case NoCompression:
              default:
              {
                (void) WriteBlobString(image,PS3_NoCompression"\n");
                break;
              }
              case RLECompression:
              {
                (void) WriteBlobString(image,PS3_RLECompression"\n");
                break;
              }
              case LZWCompression:
              {
                (void) WriteBlobString(image,PS3_LZWCompression"\n");
                break;
              }
              case ZipCompression:
              {
                (void) WriteBlobString(image,PS3_ZipCompression"\n");
                break;
              }
            }
            /*
              Number of colors in color map.
            */
            (void) FormatLocaleString(buffer,MaxTextExtent,"%.20g\n",
              (double) image->colors);
            (void) WriteBlobString(image,buffer);
            /*
              Color map - uncompressed.
            */
            if ((compression != NoCompression) &&
                (compression != UndefinedCompression))
              {
                for (i=0; i < (ssize_t) image->colors; i++)
                {
                  pixel=ScaleQuantumToChar(image->colormap[i].red);
                  (void) WriteBlobByte(image,(unsigned char) pixel);
                  pixel=ScaleQuantumToChar(image->colormap[i].green);
                  (void) WriteBlobByte(image,(unsigned char) pixel);
                  pixel=ScaleQuantumToChar(image->colormap[i].blue);
                  (void) WriteBlobByte(image,(unsigned char) pixel);
                }
              }
            else
              {
                Ascii85Initialize(image);
                for (i=0; i < (ssize_t) image->colors; i++)
                {
                  pixel=ScaleQuantumToChar(image->colormap[i].red);
                  Ascii85Encode(image,(unsigned char) pixel);
                  pixel=ScaleQuantumToChar(image->colormap[i].green);
                  Ascii85Encode(image,(unsigned char) pixel);
                  pixel=ScaleQuantumToChar(image->colormap[i].blue);
                  Ascii85Encode(image,(unsigned char) pixel);
                }
                Ascii85Flush(image);
              }
            status=SerializeImageIndexes(image_info,image,&pixel_info,&length);
            if (status == MagickFalse)
              {
                (void) CloseBlob(image);
                return(MagickFalse);
              }
            pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
            switch (compression)
            {
              case NoCompression:
              default:
              {
                Ascii85Initialize(image);
                for (i=0; i < (ssize_t) length; i++)
                  Ascii85Encode(image,pixels[i]);
                Ascii85Flush(image);
                status=MagickTrue;
                break;
              }
              case RLECompression:
              {
                status=PackbitsEncodeImage(image,length,pixels);
                break;
              }
              case LZWCompression:
              {
                status=LZWEncodeImage(image,length,pixels);
                break;
              }
              case ZipCompression:
              {
                status=ZLIBEncodeImage(image,length,pixels);
                break;
              }
            }
            pixel_info=RelinquishVirtualMemory(pixel_info);
          }
    (void) WriteBlobByte(image,'\n');
    if (status == MagickFalse)
      {
        (void) CloseBlob(image);
        return(MagickFalse);
      }
    /*
      Update BeginData now that we know the data size.
    */
    length=(size_t) (TellBlob(image)-stop);
    stop=TellBlob(image);
    if (stop < 0)
      ThrowWriterException(CorruptImageError,"ImproperImageHeader");
    offset=SeekBlob(image,start,SEEK_SET);
    if (offset < 0)
      ThrowWriterException(CorruptImageError,"ImproperImageHeader");
    (void) FormatLocaleString(buffer,MaxTextExtent,
      "%%%%BeginData:%13ld %s Bytes\n",(long) length,
      compression == NoCompression ? "ASCII" : "BINARY");
    (void) WriteBlobString(image,buffer);
    (void) SeekBlob(image,stop,SEEK_SET);
    (void) WriteBlobString(image,"%%EndData\n");
    /*
      End private dictionary if this an EPS.
    */
    if (LocaleCompare(image_info->magick,"PS3") != 0)
      (void) WriteBlobString(image,"end\n");
    (void) WriteBlobString(image,"%%PageTrailer\n");
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,imageListLength);
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  (void) WriteBlobString(image,"%%Trailer\n");
  if (page > 1)
    {
      (void) FormatLocaleString(buffer,MaxTextExtent,
        "%%%%BoundingBox: %g %g %g %g\n",ceil(bounds.x1-0.5),
        ceil(bounds.y1-0.5),floor(bounds.x2+0.5),floor(bounds.y2+0.5));
      (void) WriteBlobString(image,buffer);
      (void) FormatLocaleString(buffer,MaxTextExtent,
        "%%%%HiResBoundingBox: %g %g %g %g\n",bounds.x1,bounds.y1,bounds.x2,
        bounds.y2);
      (void) WriteBlobString(image,buffer);
    }
  (void) WriteBlobString(image,"%%EOF\n");
  (void) CloseBlob(image);
  return(MagickTrue);
}