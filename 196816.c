static MagickBooleanType WriteTIFFImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
  const char
    *mode,
    *option;

  CompressionType
    compression;

  EndianType
    endian_type;

  MagickBooleanType
    adjoin,
    preserve_compression,
    status;

  MagickOffsetType
    scene;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  ssize_t
    i;

  size_t
    imageListLength,
    length;

  ssize_t
    y;

  TIFF
    *tiff;

  TIFFInfo
    tiff_info;

  uint16
    bits_per_sample,
    compress_tag,
    endian,
    photometric,
    predictor;

  unsigned char
    *pixels;

  void
    *sans[2] = { NULL, NULL };

  /*
    Open TIFF file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  (void) SetMagickThreadValue(tiff_exception,exception);
  endian_type=(HOST_FILLORDER == FILLORDER_LSB2MSB) ? LSBEndian : MSBEndian;
  option=GetImageOption(image_info,"tiff:endian");
  if (option != (const char *) NULL)
    {
      if (LocaleNCompare(option,"msb",3) == 0)
        endian_type=MSBEndian;
      if (LocaleNCompare(option,"lsb",3) == 0)
        endian_type=LSBEndian;
    }
  mode=endian_type == LSBEndian ? "wl" : "wb";
#if defined(TIFF_VERSION_BIG)
  if (LocaleCompare(image_info->magick,"TIFF64") == 0)
    mode=endian_type == LSBEndian ? "wl8" : "wb8";
#endif
  tiff=TIFFClientOpen(image->filename,mode,(thandle_t) image,TIFFReadBlob,
    TIFFWriteBlob,TIFFSeekBlob,TIFFCloseBlob,TIFFGetBlobSize,TIFFMapBlob,
    TIFFUnmapBlob);
  if (tiff == (TIFF *) NULL)
    return(MagickFalse);
  if (exception->severity > ErrorException)
    {
      TIFFClose(tiff);
      return(MagickFalse);
    }
  (void) DeleteImageProfile(image,"tiff:37724");
  scene=0;
  adjoin=image_info->adjoin;
  imageListLength=GetImageListLength(image);
  option=GetImageOption(image_info,"tiff:preserve-compression");
  preserve_compression=IsStringTrue(option);
  do
  {
    /*
      Initialize TIFF fields.
    */
    if ((image_info->type != UndefinedType) &&
        (image_info->type != OptimizeType))
      (void) SetImageType(image,image_info->type,exception);
    compression=image_info->compression;
    if (preserve_compression != MagickFalse)
      compression=image->compression;
    switch (compression)
    {
      case FaxCompression:
      case Group4Compression:
      {
        if (IsImageMonochrome(image) == MagickFalse)
          {
            if (IsImageGray(image) == MagickFalse)
              (void) SetImageType(image,BilevelType,exception);
            else
              (void) SetImageDepth(image,1,exception);
          }
        image->depth=1;
        break;
      }
      case JPEGCompression:
      {
        (void) SetImageStorageClass(image,DirectClass,exception);
        (void) SetImageDepth(image,8,exception);
        break;
      }
      default:
        break;
    }
    quantum_info=AcquireQuantumInfo(image_info,image);
    if (quantum_info == (QuantumInfo *) NULL)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    if ((image->storage_class != PseudoClass) && (image->depth >= 32) &&
        (quantum_info->format == UndefinedQuantumFormat) &&
        (IsHighDynamicRangeImage(image,exception) != MagickFalse))
      {
        status=SetQuantumFormat(image,quantum_info,FloatingPointQuantumFormat);
        if (status == MagickFalse)
          {
            quantum_info=DestroyQuantumInfo(quantum_info);
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
          }
      }
    if ((LocaleCompare(image_info->magick,"PTIF") == 0) &&
        (GetPreviousImageInList(image) != (Image *) NULL))
      (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_REDUCEDIMAGE);
    if ((image->columns != (uint32) image->columns) ||
        (image->rows != (uint32) image->rows))
      ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
    (void) TIFFSetField(tiff,TIFFTAG_IMAGELENGTH,(uint32) image->rows);
    (void) TIFFSetField(tiff,TIFFTAG_IMAGEWIDTH,(uint32) image->columns);
    switch (compression)
    {
      case FaxCompression:
      {
        compress_tag=COMPRESSION_CCITTFAX3;
        option=GetImageOption(image_info,"quantum:polarity");
        if (option == (const char *) NULL)
          SetQuantumMinIsWhite(quantum_info,MagickTrue);
        break;
      }
      case Group4Compression:
      {
        compress_tag=COMPRESSION_CCITTFAX4;
        option=GetImageOption(image_info,"quantum:polarity");
        if (option == (const char *) NULL)
          SetQuantumMinIsWhite(quantum_info,MagickTrue);
        break;
      }
#if defined(COMPRESSION_JBIG)
      case JBIG1Compression:
      {
        compress_tag=COMPRESSION_JBIG;
        break;
      }
#endif
      case JPEGCompression:
      {
        compress_tag=COMPRESSION_JPEG;
        break;
      }
#if defined(COMPRESSION_LZMA)
      case LZMACompression:
      {
        compress_tag=COMPRESSION_LZMA;
        break;
      }
#endif
      case LZWCompression:
      {
        compress_tag=COMPRESSION_LZW;
        break;
      }
      case RLECompression:
      {
        compress_tag=COMPRESSION_PACKBITS;
        break;
      }
      case ZipCompression:
      {
        compress_tag=COMPRESSION_ADOBE_DEFLATE;
        break;
      }
#if defined(COMPRESSION_ZSTD)
      case ZstdCompression:
      {
        compress_tag=COMPRESSION_ZSTD;
        break;
      }
#endif
      case NoCompression:
      default:
      {
        compress_tag=COMPRESSION_NONE;
        break;
      }
    }
#if defined(MAGICKCORE_HAVE_TIFFISCODECCONFIGURED) || (TIFFLIB_VERSION > 20040919)
    if ((compress_tag != COMPRESSION_NONE) &&
        (TIFFIsCODECConfigured(compress_tag) == 0))
      {
        (void) ThrowMagickException(exception,GetMagickModule(),CoderError,
          "CompressionNotSupported","`%s'",CommandOptionToMnemonic(
          MagickCompressOptions,(ssize_t) compression));
        compress_tag=COMPRESSION_NONE;
        compression=NoCompression;
      }
#else
      switch (compress_tag)
      {
#if defined(CCITT_SUPPORT)
        case COMPRESSION_CCITTFAX3:
        case COMPRESSION_CCITTFAX4:
#endif
#if defined(YCBCR_SUPPORT) && defined(JPEG_SUPPORT)
        case COMPRESSION_JPEG:
#endif
#if defined(LZMA_SUPPORT) && defined(COMPRESSION_LZMA)
        case COMPRESSION_LZMA:
#endif
#if defined(LZW_SUPPORT)
        case COMPRESSION_LZW:
#endif
#if defined(PACKBITS_SUPPORT)
        case COMPRESSION_PACKBITS:
#endif
#if defined(ZIP_SUPPORT)
        case COMPRESSION_ADOBE_DEFLATE:
#endif
        case COMPRESSION_NONE:
          break;
        default:
        {
          (void) ThrowMagickException(exception,GetMagickModule(),CoderError,
            "CompressionNotSupported","`%s'",CommandOptionToMnemonic(
            MagickCompressOptions,(ssize_t) compression));
          compress_tag=COMPRESSION_NONE;
          compression=NoCompression;
          break;
        }
      }
#endif
    if (image->colorspace == CMYKColorspace)
      {
        photometric=PHOTOMETRIC_SEPARATED;
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,4);
        (void) TIFFSetField(tiff,TIFFTAG_INKSET,INKSET_CMYK);
      }
    else
      {
        /*
          Full color TIFF raster.
        */
        if (image->colorspace == LabColorspace)
          {
            photometric=PHOTOMETRIC_CIELAB;
            EncodeLabImage(image,exception);
          }
        else
          if (IsYCbCrCompatibleColorspace(image->colorspace) != MagickFalse)
            {
              photometric=PHOTOMETRIC_YCBCR;
              (void) TIFFSetField(tiff,TIFFTAG_YCBCRSUBSAMPLING,1,1);
              (void) SetImageStorageClass(image,DirectClass,exception);
              status=SetQuantumDepth(image,quantum_info,8);
              if (status == MagickFalse)
                ThrowWriterException(ResourceLimitError,
                  "MemoryAllocationFailed");
            }
          else
            photometric=PHOTOMETRIC_RGB;
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,3);
        if ((image_info->type != TrueColorType) &&
            (image_info->type != TrueColorAlphaType))
          {
            if ((image_info->type != PaletteType) &&
                (SetImageGray(image,exception) != MagickFalse))
              {
                photometric=(uint16) (quantum_info->min_is_white !=
                  MagickFalse ? PHOTOMETRIC_MINISWHITE :
                  PHOTOMETRIC_MINISBLACK);
                (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,1);
                if ((image->depth == 1) &&
                    (image->alpha_trait == UndefinedPixelTrait))
                  SetImageMonochrome(image,exception);
              }
            else
              if (image->storage_class == PseudoClass)
                {
                  size_t
                    depth;

                  /*
                    Colormapped TIFF raster.
                  */
                  (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,1);
                  photometric=PHOTOMETRIC_PALETTE;
                  depth=1;
                  while ((GetQuantumRange(depth)+1) < image->colors)
                    depth<<=1;
                  status=SetQuantumDepth(image,quantum_info,depth);
                  if (status == MagickFalse)
                    ThrowWriterException(ResourceLimitError,
                      "MemoryAllocationFailed");
                }
          }
      }
    (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_FILLORDER,&endian,sans);
    if ((compress_tag == COMPRESSION_CCITTFAX3) ||
        (compress_tag == COMPRESSION_CCITTFAX4))
      {
         if ((photometric != PHOTOMETRIC_MINISWHITE) &&
             (photometric != PHOTOMETRIC_MINISBLACK))
          {
            compress_tag=COMPRESSION_NONE;
            endian=FILLORDER_MSB2LSB;
          }
      }
    option=GetImageOption(image_info,"tiff:fill-order");
    if (option != (const char *) NULL)
      {
        if (LocaleNCompare(option,"msb",3) == 0)
          endian=FILLORDER_MSB2LSB;
        if (LocaleNCompare(option,"lsb",3) == 0)
          endian=FILLORDER_LSB2MSB;
      }
    (void) TIFFSetField(tiff,TIFFTAG_COMPRESSION,compress_tag);
    (void) TIFFSetField(tiff,TIFFTAG_FILLORDER,endian);
    (void) TIFFSetField(tiff,TIFFTAG_BITSPERSAMPLE,quantum_info->depth);
    if (image->alpha_trait != UndefinedPixelTrait)
      {
        uint16
          extra_samples,
          sample_info[1],
          samples_per_pixel;

        /*
          TIFF has a matte channel.
        */
        extra_samples=1;
        sample_info[0]=EXTRASAMPLE_UNASSALPHA;
        option=GetImageOption(image_info,"tiff:alpha");
        if (option != (const char *) NULL)
          {
            if (LocaleCompare(option,"associated") == 0)
              sample_info[0]=EXTRASAMPLE_ASSOCALPHA;
            else
              if (LocaleCompare(option,"unspecified") == 0)
                sample_info[0]=EXTRASAMPLE_UNSPECIFIED;
          }
        (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLESPERPIXEL,
          &samples_per_pixel,sans);
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLESPERPIXEL,samples_per_pixel+1);
        (void) TIFFSetField(tiff,TIFFTAG_EXTRASAMPLES,extra_samples,
          &sample_info);
        if (sample_info[0] == EXTRASAMPLE_ASSOCALPHA)
          SetQuantumAlphaType(quantum_info,AssociatedQuantumAlpha);
      }
    (void) TIFFSetField(tiff,TIFFTAG_PHOTOMETRIC,photometric);
    switch (quantum_info->format)
    {
      case FloatingPointQuantumFormat:
      {
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_IEEEFP);
        (void) TIFFSetField(tiff,TIFFTAG_SMINSAMPLEVALUE,quantum_info->minimum);
        (void) TIFFSetField(tiff,TIFFTAG_SMAXSAMPLEVALUE,quantum_info->maximum);
        break;
      }
      case SignedQuantumFormat:
      {
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_INT);
        break;
      }
      case UnsignedQuantumFormat:
      {
        (void) TIFFSetField(tiff,TIFFTAG_SAMPLEFORMAT,SAMPLEFORMAT_UINT);
        break;
      }
      default:
        break;
    }
    (void) TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
    if (photometric == PHOTOMETRIC_RGB)
      if ((image_info->interlace == PlaneInterlace) ||
          (image_info->interlace == PartitionInterlace))
        (void) TIFFSetField(tiff,TIFFTAG_PLANARCONFIG,PLANARCONFIG_SEPARATE);
    predictor=0;
    switch (compress_tag)
    {
      case COMPRESSION_JPEG:
      {
#if defined(JPEG_SUPPORT)
        if (image_info->quality != UndefinedCompressionQuality)
          (void) TIFFSetField(tiff,TIFFTAG_JPEGQUALITY,image_info->quality);
        (void) TIFFSetField(tiff,TIFFTAG_JPEGCOLORMODE,JPEGCOLORMODE_RAW);
        if (IssRGBCompatibleColorspace(image->colorspace) != MagickFalse)
          {
            const char
              *value;

            (void) TIFFSetField(tiff,TIFFTAG_JPEGCOLORMODE,JPEGCOLORMODE_RGB);
            if (IsYCbCrCompatibleColorspace(image->colorspace) != MagickFalse)
              {
                const char
                  *sampling_factor;

                GeometryInfo
                  geometry_info;

                MagickStatusType
                  flags;

                sampling_factor=(const char *) NULL;
                value=GetImageProperty(image,"jpeg:sampling-factor",exception);
                if (value != (char *) NULL)
                  {
                    sampling_factor=value;
                    if (image->debug != MagickFalse)
                      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                        "  Input sampling-factors=%s",sampling_factor);
                  }
                if (image_info->sampling_factor != (char *) NULL)
                  sampling_factor=image_info->sampling_factor;
                if (sampling_factor != (const char *) NULL)
                  {
                    flags=ParseGeometry(sampling_factor,&geometry_info);
                    if ((flags & SigmaValue) == 0)
                      geometry_info.sigma=geometry_info.rho;
                    (void) TIFFSetField(tiff,TIFFTAG_YCBCRSUBSAMPLING,(uint16)
                      geometry_info.rho,(uint16) geometry_info.sigma);
                  }
                }
          }
        (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,
          &bits_per_sample,sans);
        if (bits_per_sample == 12)
          (void) TIFFSetField(tiff,TIFFTAG_JPEGTABLESMODE,JPEGTABLESMODE_QUANT);
#endif
        break;
      }
      case COMPRESSION_ADOBE_DEFLATE:
      {
        (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,
          &bits_per_sample,sans);
        if (((photometric == PHOTOMETRIC_RGB) ||
             (photometric == PHOTOMETRIC_SEPARATED) ||
             (photometric == PHOTOMETRIC_MINISBLACK)) &&
            ((bits_per_sample == 8) || (bits_per_sample == 16)))
          predictor=PREDICTOR_HORIZONTAL;
        (void) TIFFSetField(tiff,TIFFTAG_ZIPQUALITY,(long) (
          image_info->quality == UndefinedCompressionQuality ? 7 :
          MagickMin((ssize_t) image_info->quality/10,9)));
        break;
      }
      case COMPRESSION_CCITTFAX3:
      {
        /*
          Byte-aligned EOL.
        */
        (void) TIFFSetField(tiff,TIFFTAG_GROUP3OPTIONS,4);
        break;
      }
      case COMPRESSION_CCITTFAX4:
        break;
#if defined(LZMA_SUPPORT) && defined(COMPRESSION_LZMA)
      case COMPRESSION_LZMA:
      {
        if (((photometric == PHOTOMETRIC_RGB) ||
             (photometric == PHOTOMETRIC_SEPARATED) ||
             (photometric == PHOTOMETRIC_MINISBLACK)) &&
            ((bits_per_sample == 8) || (bits_per_sample == 16)))
          predictor=PREDICTOR_HORIZONTAL;
        (void) TIFFSetField(tiff,TIFFTAG_LZMAPRESET,(long) (
          image_info->quality == UndefinedCompressionQuality ? 7 :
          MagickMin((ssize_t) image_info->quality/10,9)));
        break;
      }
#endif
      case COMPRESSION_LZW:
      {
        (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,
          &bits_per_sample,sans);
        if (((photometric == PHOTOMETRIC_RGB) ||
             (photometric == PHOTOMETRIC_SEPARATED) ||
             (photometric == PHOTOMETRIC_MINISBLACK)) &&
            ((bits_per_sample == 8) || (bits_per_sample == 16)))
          predictor=PREDICTOR_HORIZONTAL;
        break;
      }
#if defined(WEBP_SUPPORT) && defined(COMPRESSION_WEBP)
      case COMPRESSION_WEBP:
      {
        (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,
          &bits_per_sample,sans);
        if (((photometric == PHOTOMETRIC_RGB) ||
             (photometric == PHOTOMETRIC_SEPARATED) ||
             (photometric == PHOTOMETRIC_MINISBLACK)) &&
            ((bits_per_sample == 8) || (bits_per_sample == 16)))
          predictor=PREDICTOR_HORIZONTAL;
        (void) TIFFSetField(tiff,TIFFTAG_WEBP_LEVEL,image_info->quality);
        if (image_info->quality >= 100)
          (void) TIFFSetField(tiff,TIFFTAG_WEBP_LOSSLESS,1);
        break;
      }
#endif
#if defined(ZSTD_SUPPORT) && defined(COMPRESSION_ZSTD)
      case COMPRESSION_ZSTD:
      {
        (void) TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,
          &bits_per_sample,sans);
        if (((photometric == PHOTOMETRIC_RGB) ||
             (photometric == PHOTOMETRIC_SEPARATED) ||
             (photometric == PHOTOMETRIC_MINISBLACK)) &&
            ((bits_per_sample == 8) || (bits_per_sample == 16)))
          predictor=PREDICTOR_HORIZONTAL;
        (void) TIFFSetField(tiff,TIFFTAG_ZSTD_LEVEL,22*image_info->quality/
          100.0);
        break;
      }
#endif
      default:
        break;
    }
    if (quantum_info->format == FloatingPointQuantumFormat)
      predictor=PREDICTOR_FLOATINGPOINT;
    option=GetImageOption(image_info,"tiff:predictor");
    if (option != (const char * ) NULL)
      predictor=(uint16) strtol(option,(char **) NULL,10);
    if (predictor != 0)
      (void) TIFFSetField(tiff,TIFFTAG_PREDICTOR,predictor);
    if ((image->resolution.x != 0.0) && (image->resolution.y != 0.0))
      {
        unsigned short
          units;

        /*
          Set image resolution.
        */
        units=RESUNIT_NONE;
        if (image->units == PixelsPerInchResolution)
          units=RESUNIT_INCH;
        if (image->units == PixelsPerCentimeterResolution)
          units=RESUNIT_CENTIMETER;
        (void) TIFFSetField(tiff,TIFFTAG_RESOLUTIONUNIT,(uint16) units);
        (void) TIFFSetField(tiff,TIFFTAG_XRESOLUTION,image->resolution.x);
        (void) TIFFSetField(tiff,TIFFTAG_YRESOLUTION,image->resolution.y);
        if ((image->page.x < 0) || (image->page.y < 0))
          (void) ThrowMagickException(exception,GetMagickModule(),CoderError,
            "TIFF: negative image positions unsupported","%s",image->filename);
        if ((image->page.x > 0) && (image->resolution.x > 0.0))
          {
            /*
              Set horizontal image position.
            */
            (void) TIFFSetField(tiff,TIFFTAG_XPOSITION,(float) image->page.x/
              image->resolution.x);
          }
        if ((image->page.y > 0) && (image->resolution.y > 0.0))
          {
            /*
              Set vertical image position.
            */
            (void) TIFFSetField(tiff,TIFFTAG_YPOSITION,(float) image->page.y/
              image->resolution.y);
          }
      }
    if (image->chromaticity.white_point.x != 0.0)
      {
        float
          chromaticity[6];

        /*
          Set image chromaticity.
        */
        chromaticity[0]=(float) image->chromaticity.red_primary.x;
        chromaticity[1]=(float) image->chromaticity.red_primary.y;
        chromaticity[2]=(float) image->chromaticity.green_primary.x;
        chromaticity[3]=(float) image->chromaticity.green_primary.y;
        chromaticity[4]=(float) image->chromaticity.blue_primary.x;
        chromaticity[5]=(float) image->chromaticity.blue_primary.y;
        (void) TIFFSetField(tiff,TIFFTAG_PRIMARYCHROMATICITIES,chromaticity);
        chromaticity[0]=(float) image->chromaticity.white_point.x;
        chromaticity[1]=(float) image->chromaticity.white_point.y;
        (void) TIFFSetField(tiff,TIFFTAG_WHITEPOINT,chromaticity);
      }
    option=GetImageOption(image_info,"tiff:write-layers");
    if (IsStringTrue(option) != MagickFalse)
      {
        (void) TIFFWritePhotoshopLayers(image,image_info,endian_type,exception);
        adjoin=MagickFalse;
      }
    if ((LocaleCompare(image_info->magick,"PTIF") != 0) &&
        (adjoin != MagickFalse) && (imageListLength > 1))
      {
        (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_PAGE);
        if (image->scene != 0)
          (void) TIFFSetField(tiff,TIFFTAG_PAGENUMBER,(uint16) image->scene,
            imageListLength);
      }
    if (image->orientation != UndefinedOrientation)
      (void) TIFFSetField(tiff,TIFFTAG_ORIENTATION,(uint16) image->orientation);
    else
      (void) TIFFSetField(tiff,TIFFTAG_ORIENTATION,ORIENTATION_TOPLEFT);
    TIFFSetProfiles(tiff,image);
    {
      uint16
        page,
        pages;

      page=(uint16) scene;
      pages=(uint16) imageListLength;
      if ((LocaleCompare(image_info->magick,"PTIF") != 0) &&
          (adjoin != MagickFalse) && (pages > 1))
        (void) TIFFSetField(tiff,TIFFTAG_SUBFILETYPE,FILETYPE_PAGE);
      (void) TIFFSetField(tiff,TIFFTAG_PAGENUMBER,page,pages);
    }
    (void) TIFFSetProperties(tiff,adjoin,image,exception);
    /*
      Write image scanlines.
    */
    if (GetTIFFInfo(image_info,tiff,&tiff_info) == MagickFalse)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    if (compress_tag == COMPRESSION_CCITTFAX4)
      (void) TIFFSetField(tiff,TIFFTAG_ROWSPERSTRIP,(uint32) image->rows);
    quantum_info->endian=LSBEndian;
    pixels=(unsigned char *) GetQuantumPixels(quantum_info);
    tiff_info.scanline=(unsigned char *) GetQuantumPixels(quantum_info);
    switch (photometric)
    {
      case PHOTOMETRIC_CIELAB:
      case PHOTOMETRIC_YCBCR:
      case PHOTOMETRIC_RGB:
      {
        /*
          RGB TIFF image.
        */
        switch (image_info->interlace)
        {
          case NoInterlace:
          default:
          {
            quantum_type=RGBQuantum;
            if (image->alpha_trait != UndefinedPixelTrait)
              quantum_type=RGBAQuantum;
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              const Quantum
                *magick_restrict p;

              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              length=ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
                quantum_type,pixels,exception);
              (void) length;
              if (TIFFWritePixels(tiff,&tiff_info,y,0,image) == -1)
                break;
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,SaveImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case PlaneInterlace:
          case PartitionInterlace:
          {
            /*
              Plane interlacing:  RRRRRR...GGGGGG...BBBBBB...
            */
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              const Quantum
                *magick_restrict p;

              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              length=ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
                RedQuantum,pixels,exception);
              if (TIFFWritePixels(tiff,&tiff_info,y,0,image) == -1)
                break;
            }
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,SaveImageTag,100,400);
                if (status == MagickFalse)
                  break;
              }
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              const Quantum
                *magick_restrict p;

              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              length=ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
                GreenQuantum,pixels,exception);
              if (TIFFWritePixels(tiff,&tiff_info,y,1,image) == -1)
                break;
            }
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,SaveImageTag,200,400);
                if (status == MagickFalse)
                  break;
              }
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              const Quantum
                *magick_restrict p;

              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              length=ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
                BlueQuantum,pixels,exception);
              if (TIFFWritePixels(tiff,&tiff_info,y,2,image) == -1)
                break;
            }
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,SaveImageTag,300,400);
                if (status == MagickFalse)
                  break;
              }
            if (image->alpha_trait != UndefinedPixelTrait)
              for (y=0; y < (ssize_t) image->rows; y++)
              {
                const Quantum
                  *magick_restrict p;

                p=GetVirtualPixels(image,0,y,image->columns,1,exception);
                if (p == (const Quantum *) NULL)
                  break;
                length=ExportQuantumPixels(image,(CacheView *) NULL,
                  quantum_info,AlphaQuantum,pixels,exception);
                if (TIFFWritePixels(tiff,&tiff_info,y,3,image) == -1)
                  break;
              }
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,SaveImageTag,400,400);
                if (status == MagickFalse)
                  break;
              }
            break;
          }
        }
        break;
      }
      case PHOTOMETRIC_SEPARATED:
      {
        /*
          CMYK TIFF image.
        */
        quantum_type=CMYKQuantum;
        if (image->alpha_trait != UndefinedPixelTrait)
          quantum_type=CMYKAQuantum;
        if (image->colorspace != CMYKColorspace)
          (void) TransformImageColorspace(image,CMYKColorspace,exception);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          const Quantum
            *magick_restrict p;

          p=GetVirtualPixels(image,0,y,image->columns,1,exception);
          if (p == (const Quantum *) NULL)
            break;
          length=ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          if (TIFFWritePixels(tiff,&tiff_info,y,0,image) == -1)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        break;
      }
      case PHOTOMETRIC_PALETTE:
      {
        uint16
          *blue,
          *green,
          *red;

        /*
          Colormapped TIFF image.
        */
        red=(uint16 *) AcquireQuantumMemory(65536,sizeof(*red));
        green=(uint16 *) AcquireQuantumMemory(65536,sizeof(*green));
        blue=(uint16 *) AcquireQuantumMemory(65536,sizeof(*blue));
        if ((red == (uint16 *) NULL) || (green == (uint16 *) NULL) ||
            (blue == (uint16 *) NULL))
          {
            if (red != (uint16 *) NULL)
              red=(uint16 *) RelinquishMagickMemory(red);
            if (green != (uint16 *) NULL)
              green=(uint16 *) RelinquishMagickMemory(green);
            if (blue != (uint16 *) NULL)
              blue=(uint16 *) RelinquishMagickMemory(blue);
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
          }
        /*
          Initialize TIFF colormap.
        */
        (void) memset(red,0,65536*sizeof(*red));
        (void) memset(green,0,65536*sizeof(*green));
        (void) memset(blue,0,65536*sizeof(*blue));
        for (i=0; i < (ssize_t) image->colors; i++)
        {
          red[i]=ScaleQuantumToShort(image->colormap[i].red);
          green[i]=ScaleQuantumToShort(image->colormap[i].green);
          blue[i]=ScaleQuantumToShort(image->colormap[i].blue);
        }
        (void) TIFFSetField(tiff,TIFFTAG_COLORMAP,red,green,blue);
        red=(uint16 *) RelinquishMagickMemory(red);
        green=(uint16 *) RelinquishMagickMemory(green);
        blue=(uint16 *) RelinquishMagickMemory(blue);
      }
      default:
      {
        /*
          Convert PseudoClass packets to contiguous grayscale scanlines.
        */
        quantum_type=IndexQuantum;
        if (image->alpha_trait != UndefinedPixelTrait)
          {
            if (photometric != PHOTOMETRIC_PALETTE)
              quantum_type=GrayAlphaQuantum;
            else
              quantum_type=IndexAlphaQuantum;
           }
         else
           if (photometric != PHOTOMETRIC_PALETTE)
             quantum_type=GrayQuantum;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          const Quantum
            *magick_restrict p;

          p=GetVirtualPixels(image,0,y,image->columns,1,exception);
          if (p == (const Quantum *) NULL)
            break;
          length=ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          if (TIFFWritePixels(tiff,&tiff_info,y,0,image) == -1)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        break;
      }
    }
    quantum_info=DestroyQuantumInfo(quantum_info);
    if (image->colorspace == LabColorspace)
      DecodeLabImage(image,exception);
    DestroyTIFFInfo(&tiff_info);
    /* TIFFPrintDirectory(tiff,stdout,MagickFalse); */
    if (TIFFWriteDirectory(tiff) == 0)
      {
        status=MagickFalse;
        break;
      }
    image=SyncNextImageInList(image);
    if (image == (Image *) NULL)
      break;
    status=SetImageProgress(image,SaveImagesTag,scene++,imageListLength);
    if (status == MagickFalse)
      break;
  } while (adjoin != MagickFalse);
  TIFFClose(tiff);
  return(status);
}