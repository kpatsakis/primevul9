static MagickBooleanType WritePNGImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
  MagickBooleanType
    excluding,
    logging,
    status;

  MngInfo
    *mng_info;

  const char
    *value;

  int
    source;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"Enter WritePNGImage()");
  /*
    Allocate a MngInfo structure.
  */
  mng_info=(MngInfo *) AcquireMagickMemory(sizeof(MngInfo));

  if (mng_info == (MngInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");

  /*
    Initialize members of the MngInfo structure.
  */
  (void) memset(mng_info,0,sizeof(MngInfo));
  mng_info->image=image;
  mng_info->equal_backgrounds=MagickTrue;

  /* See if user has requested a specific PNG subformat */

  mng_info->write_png8=LocaleCompare(image_info->magick,"PNG8") == 0;
  mng_info->write_png24=LocaleCompare(image_info->magick,"PNG24") == 0;
  mng_info->write_png32=LocaleCompare(image_info->magick,"PNG32") == 0;
  mng_info->write_png48=LocaleCompare(image_info->magick,"PNG48") == 0;
  mng_info->write_png64=LocaleCompare(image_info->magick,"PNG64") == 0;

  value=GetImageOption(image_info,"png:format");

  if (value != (char *) NULL || LocaleCompare(image_info->magick,"PNG00") == 0)
    {
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
         "  Format=%s",value);

      mng_info->write_png8 = MagickFalse;
      mng_info->write_png24 = MagickFalse;
      mng_info->write_png32 = MagickFalse;
      mng_info->write_png48 = MagickFalse;
      mng_info->write_png64 = MagickFalse;

      if (LocaleCompare(value,"png8") == 0)
        mng_info->write_png8 = MagickTrue;

      else if (LocaleCompare(value,"png24") == 0)
        mng_info->write_png24 = MagickTrue;

      else if (LocaleCompare(value,"png32") == 0)
        mng_info->write_png32 = MagickTrue;

      else if (LocaleCompare(value,"png48") == 0)
        mng_info->write_png48 = MagickTrue;

      else if (LocaleCompare(value,"png64") == 0)
        mng_info->write_png64 = MagickTrue;

      else if ((LocaleCompare(value,"png00") == 0) ||
         LocaleCompare(image_info->magick,"PNG00") == 0)
        {
          /* Retrieve png:IHDR.bit-depth-orig and png:IHDR.color-type-orig. */
          value=GetImageProperty(image,"png:IHDR.bit-depth-orig",exception);

          if (value != (char *) NULL)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "  png00 inherited bit depth=%s",value);

              if (LocaleCompare(value,"1") == 0)
                mng_info->write_png_depth = 1;

              else if (LocaleCompare(value,"2") == 0)
                mng_info->write_png_depth = 2;

              else if (LocaleCompare(value,"4") == 0)
                mng_info->write_png_depth = 4;

              else if (LocaleCompare(value,"8") == 0)
                mng_info->write_png_depth = 8;

              else if (LocaleCompare(value,"16") == 0)
                mng_info->write_png_depth = 16;
            }

          value=GetImageProperty(image,"png:IHDR.color-type-orig",exception);

          if (value != (char *) NULL)
            {
              (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "  png00 inherited color type=%s",value);

              if (LocaleCompare(value,"0") == 0)
                mng_info->write_png_colortype = 1;

              else if (LocaleCompare(value,"2") == 0)
                mng_info->write_png_colortype = 3;

              else if (LocaleCompare(value,"3") == 0)
                mng_info->write_png_colortype = 4;

              else if (LocaleCompare(value,"4") == 0)
                mng_info->write_png_colortype = 5;

              else if (LocaleCompare(value,"6") == 0)
                mng_info->write_png_colortype = 7;
            }
        }
    }

  if (mng_info->write_png8)
    {
      mng_info->write_png_colortype = /* 3 */ 4;
      mng_info->write_png_depth = 8;
      image->depth = 8;
    }

  if (mng_info->write_png24)
    {
      mng_info->write_png_colortype = /* 2 */ 3;
      mng_info->write_png_depth = 8;
      image->depth = 8;

      if (image->alpha_trait != UndefinedPixelTrait)
        (void) SetImageType(image,TrueColorAlphaType,exception);

      else
        (void) SetImageType(image,TrueColorType,exception);

      (void) SyncImage(image,exception);
    }

  if (mng_info->write_png32)
    {
      mng_info->write_png_colortype = /* 6 */  7;
      mng_info->write_png_depth = 8;
      image->depth = 8;
      image->alpha_trait = BlendPixelTrait;

      (void) SetImageType(image,TrueColorAlphaType,exception);
      (void) SyncImage(image,exception);
    }

  if (mng_info->write_png48)
    {
      mng_info->write_png_colortype = /* 2 */ 3;
      mng_info->write_png_depth = 16;
      image->depth = 16;

      if (image->alpha_trait != UndefinedPixelTrait)
        (void) SetImageType(image,TrueColorAlphaType,exception);

      else
        (void) SetImageType(image,TrueColorType,exception);

      (void) SyncImage(image,exception);
    }

  if (mng_info->write_png64)
    {
      mng_info->write_png_colortype = /* 6 */  7;
      mng_info->write_png_depth = 16;
      image->depth = 16;
      image->alpha_trait = BlendPixelTrait;

      (void) SetImageType(image,TrueColorAlphaType,exception);
      (void) SyncImage(image,exception);
    }

  value=GetImageOption(image_info,"png:bit-depth");

  if (value != (char *) NULL)
    {
      if (LocaleCompare(value,"1") == 0)
        mng_info->write_png_depth = 1;

      else if (LocaleCompare(value,"2") == 0)
        mng_info->write_png_depth = 2;

      else if (LocaleCompare(value,"4") == 0)
        mng_info->write_png_depth = 4;

      else if (LocaleCompare(value,"8") == 0)
        mng_info->write_png_depth = 8;

      else if (LocaleCompare(value,"16") == 0)
        mng_info->write_png_depth = 16;

      else
        (void) ThrowMagickException(exception,
             GetMagickModule(),CoderWarning,
             "ignoring invalid defined png:bit-depth",
             "=%s",value);

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  png:bit-depth=%d was defined.\n",mng_info->write_png_depth);
    }

  value=GetImageOption(image_info,"png:color-type");

  if (value != (char *) NULL)
    {
      /* We must store colortype+1 because 0 is a valid colortype */
      if (LocaleCompare(value,"0") == 0)
        mng_info->write_png_colortype = 1;

      else if (LocaleCompare(value,"1") == 0)
        mng_info->write_png_colortype = 2;

      else if (LocaleCompare(value,"2") == 0)
        mng_info->write_png_colortype = 3;

      else if (LocaleCompare(value,"3") == 0)
        mng_info->write_png_colortype = 4;

      else if (LocaleCompare(value,"4") == 0)
        mng_info->write_png_colortype = 5;

      else if (LocaleCompare(value,"6") == 0)
        mng_info->write_png_colortype = 7;

      else
        (void) ThrowMagickException(exception,
             GetMagickModule(),CoderWarning,
             "ignoring invalid defined png:color-type",
             "=%s",value);

      if (logging != MagickFalse)
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "  png:color-type=%d was defined.\n",
          mng_info->write_png_colortype-1);
    }

  /* Check for chunks to be excluded:
   *
   * The default is to not exclude any known chunks except for any
   * listed in the "unused_chunks" array, above.
   *
   * Chunks can be listed for exclusion via a "png:exclude-chunk"
   * define (in the image properties or in the image artifacts)
   * or via a mng_info member.  For convenience, in addition
   * to or instead of a comma-separated list of chunks, the
   * "exclude-chunk" string can be simply "all" or "none".
   *
   * Note that the "-strip" option provides a convenient way of
   * doing the equivalent of
   *
   *    -define png:exclude-chunk="bKGD,caNv,cHRM,eXIf,gAMA,iCCP,
   *            iTXt,pHYs,sRGB,tEXt,zCCP,zTXt,date"
   *
   * The exclude-chunk define takes priority over the mng_info.
   *
   * A "png:include-chunk" define takes  priority over both the
   * mng_info and the "png:exclude-chunk" define.  Like the
   * "exclude-chunk" string, it can define "all" or "none" as
   * well as a comma-separated list.  Chunks that are unknown to
   * ImageMagick are always excluded, regardless of their "copy-safe"
   * status according to the PNG specification, and even if they
   * appear in the "include-chunk" list. Such defines appearing among
   * the image options take priority over those found among the image
   * artifacts.
   *
   * Finally, all chunks listed in the "unused_chunks" array are
   * automatically excluded, regardless of the other instructions
   * or lack thereof.
   *
   * if you exclude sRGB but not gAMA (recommended), then sRGB chunk
   * will not be written and the gAMA chunk will only be written if it
   * is not between .45 and .46, or approximately (1.0/2.2).
   *
   * If you exclude tRNS and the image has transparency, the colortype
   * is forced to be 4 or 6 (GRAY_ALPHA or RGB_ALPHA).
   *
   * The -strip option causes StripImage() to set the png:include-chunk
   * artifact to "none,trns,gama".
   */

  mng_info->ping_exclude_bKGD=MagickFalse;
  mng_info->ping_exclude_caNv=MagickFalse;
  mng_info->ping_exclude_cHRM=MagickFalse;
  mng_info->ping_exclude_date=MagickFalse;
  mng_info->ping_exclude_eXIf=MagickFalse;
  mng_info->ping_exclude_EXIF=MagickFalse; /* hex-encoded EXIF in zTXt */
  mng_info->ping_exclude_gAMA=MagickFalse;
  mng_info->ping_exclude_iCCP=MagickFalse;
  /* mng_info->ping_exclude_iTXt=MagickFalse; */
  mng_info->ping_exclude_oFFs=MagickFalse;
  mng_info->ping_exclude_pHYs=MagickFalse;
  mng_info->ping_exclude_sRGB=MagickFalse;
  mng_info->ping_exclude_tEXt=MagickFalse;
  mng_info->ping_exclude_tIME=MagickFalse;
  mng_info->ping_exclude_tRNS=MagickFalse;
  mng_info->ping_exclude_zCCP=MagickFalse; /* hex-encoded iCCP in zTXt */
  mng_info->ping_exclude_zTXt=MagickFalse;

  mng_info->ping_preserve_colormap=MagickFalse;

  value=GetImageOption(image_info,"png:preserve-colormap");
  if (value == NULL)
     value=GetImageArtifact(image,"png:preserve-colormap");
  if (value != NULL)
     mng_info->ping_preserve_colormap=MagickTrue;

  mng_info->ping_preserve_iCCP=MagickFalse;

  value=GetImageOption(image_info,"png:preserve-iCCP");
  if (value == NULL)
     value=GetImageArtifact(image,"png:preserve-iCCP");
  if (value != NULL)
     mng_info->ping_preserve_iCCP=MagickTrue;

  /* These compression-level, compression-strategy, and compression-filter
   * defines take precedence over values from the -quality option.
   */
  value=GetImageOption(image_info,"png:compression-level");
  if (value == NULL)
     value=GetImageArtifact(image,"png:compression-level");
  if (value != NULL)
  {
      /* We have to add 1 to everything because 0 is a valid input,
       * and we want to use 0 (the default) to mean undefined.
       */
      if (LocaleCompare(value,"0") == 0)
        mng_info->write_png_compression_level = 1;

      else if (LocaleCompare(value,"1") == 0)
        mng_info->write_png_compression_level = 2;

      else if (LocaleCompare(value,"2") == 0)
        mng_info->write_png_compression_level = 3;

      else if (LocaleCompare(value,"3") == 0)
        mng_info->write_png_compression_level = 4;

      else if (LocaleCompare(value,"4") == 0)
        mng_info->write_png_compression_level = 5;

      else if (LocaleCompare(value,"5") == 0)
        mng_info->write_png_compression_level = 6;

      else if (LocaleCompare(value,"6") == 0)
        mng_info->write_png_compression_level = 7;

      else if (LocaleCompare(value,"7") == 0)
        mng_info->write_png_compression_level = 8;

      else if (LocaleCompare(value,"8") == 0)
        mng_info->write_png_compression_level = 9;

      else if (LocaleCompare(value,"9") == 0)
        mng_info->write_png_compression_level = 10;

      else
        (void) ThrowMagickException(exception,
             GetMagickModule(),CoderWarning,
             "ignoring invalid defined png:compression-level",
             "=%s",value);
    }

  value=GetImageOption(image_info,"png:compression-strategy");
  if (value == NULL)
     value=GetImageArtifact(image,"png:compression-strategy");
  if (value != NULL)
  {
      if (LocaleCompare(value,"0") == 0)
        mng_info->write_png_compression_strategy = Z_DEFAULT_STRATEGY+1;

      else if (LocaleCompare(value,"1") == 0)
        mng_info->write_png_compression_strategy = Z_FILTERED+1;

      else if (LocaleCompare(value,"2") == 0)
        mng_info->write_png_compression_strategy = Z_HUFFMAN_ONLY+1;

      else if (LocaleCompare(value,"3") == 0)
#ifdef Z_RLE  /* Z_RLE was added to zlib-1.2.0 */
        mng_info->write_png_compression_strategy = Z_RLE+1;
#else
        mng_info->write_png_compression_strategy = Z_DEFAULT_STRATEGY+1;
#endif

      else if (LocaleCompare(value,"4") == 0)
#ifdef Z_FIXED  /* Z_FIXED was added to zlib-1.2.2.2 */
        mng_info->write_png_compression_strategy = Z_FIXED+1;
#else
        mng_info->write_png_compression_strategy = Z_DEFAULT_STRATEGY+1;
#endif

      else
        (void) ThrowMagickException(exception,
             GetMagickModule(),CoderWarning,
             "ignoring invalid defined png:compression-strategy",
             "=%s",value);
    }

  value=GetImageOption(image_info,"png:compression-filter");
  if (value == NULL)
     value=GetImageArtifact(image,"png:compression-filter");
  if (value != NULL)
  {
      /* To do: combinations of filters allowed by libpng
       * masks 0x08 through 0xf8
       *
       * Implement this as a comma-separated list of 0,1,2,3,4,5
       * where 5 is a special case meaning PNG_ALL_FILTERS.
       */

      if (LocaleCompare(value,"0") == 0)
        mng_info->write_png_compression_filter = 1;

      else if (LocaleCompare(value,"1") == 0)
        mng_info->write_png_compression_filter = 2;

      else if (LocaleCompare(value,"2") == 0)
        mng_info->write_png_compression_filter = 3;

      else if (LocaleCompare(value,"3") == 0)
        mng_info->write_png_compression_filter = 4;

      else if (LocaleCompare(value,"4") == 0)
        mng_info->write_png_compression_filter = 5;

      else if (LocaleCompare(value,"5") == 0)
        mng_info->write_png_compression_filter = 6;

      else
        (void) ThrowMagickException(exception,
             GetMagickModule(),CoderWarning,
             "ignoring invalid defined png:compression-filter",
             "=%s",value);
  }

  for (source=0; source<8; source++)
  {
    value = NULL;

    if (source == 0)
      value=GetImageOption(image_info,"png:exclude-chunks");

    if (source == 1)
      value=GetImageArtifact(image,"png:exclude-chunks");

    if (source == 2)
      value=GetImageOption(image_info,"png:exclude-chunk");

    if (source == 3)
      value=GetImageArtifact(image,"png:exclude-chunk");

    if (source == 4)
      value=GetImageOption(image_info,"png:include-chunks");

    if (source == 5)
      value=GetImageArtifact(image,"png:include-chunks");

    if (source == 6)
      value=GetImageOption(image_info,"png:include-chunk");

    if (source == 7)
      value=GetImageArtifact(image,"png:include-chunk");

    if (value == NULL)
       continue;

    if (source < 4)
      excluding = MagickTrue;
    else
      excluding = MagickFalse;

    if (logging != MagickFalse)
      {
        if (source == 0 || source == 2)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  png:exclude-chunk=%s found in image options.\n", value);
        else if (source == 1 || source == 3)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  png:exclude-chunk=%s found in image artifacts.\n", value);
        else if (source == 4 || source == 6)
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  png:include-chunk=%s found in image options.\n", value);
        else /* if (source == 5 || source == 7) */
           (void) LogMagickEvent(CoderEvent,GetMagickModule(),
              "  png:include-chunk=%s found in image artifacts.\n", value);
      }

    if (IsOptionMember("all",value) != MagickFalse)
      {
        mng_info->ping_exclude_bKGD=excluding;
        mng_info->ping_exclude_caNv=excluding;
        mng_info->ping_exclude_cHRM=excluding;
        mng_info->ping_exclude_date=excluding;
        mng_info->ping_exclude_EXIF=excluding;
        mng_info->ping_exclude_eXIf=excluding;
        mng_info->ping_exclude_gAMA=excluding;
        mng_info->ping_exclude_iCCP=excluding;
        /* mng_info->ping_exclude_iTXt=excluding; */
        mng_info->ping_exclude_oFFs=excluding;
        mng_info->ping_exclude_pHYs=excluding;
        mng_info->ping_exclude_sRGB=excluding;
        mng_info->ping_exclude_tEXt=excluding;
        mng_info->ping_exclude_tIME=excluding;
        mng_info->ping_exclude_tRNS=excluding;
        mng_info->ping_exclude_zCCP=excluding;
        mng_info->ping_exclude_zTXt=excluding;
      }

    if (IsOptionMember("none",value) != MagickFalse)
      {
        mng_info->ping_exclude_bKGD=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_caNv=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_cHRM=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_date=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_eXIf=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_EXIF=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_gAMA=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_iCCP=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        /* mng_info->ping_exclude_iTXt=!excluding; */
        mng_info->ping_exclude_oFFs=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_pHYs=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_sRGB=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_tEXt=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_tIME=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_tRNS=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_zCCP=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
        mng_info->ping_exclude_zTXt=excluding != MagickFalse ? MagickFalse :
          MagickTrue;
      }

    if (IsOptionMember("bkgd",value) != MagickFalse)
      mng_info->ping_exclude_bKGD=excluding;

    if (IsOptionMember("caNv",value) != MagickFalse)
      mng_info->ping_exclude_caNv=excluding;

    if (IsOptionMember("chrm",value) != MagickFalse)
      mng_info->ping_exclude_cHRM=excluding;

    if (IsOptionMember("date",value) != MagickFalse)
      mng_info->ping_exclude_date=excluding;

    if (IsOptionMember("exif",value) != MagickFalse)
      {
        mng_info->ping_exclude_EXIF=excluding;
        mng_info->ping_exclude_eXIf=excluding;
      }

    if (IsOptionMember("gama",value) != MagickFalse)
      mng_info->ping_exclude_gAMA=excluding;

    if (IsOptionMember("iccp",value) != MagickFalse)
      mng_info->ping_exclude_iCCP=excluding;

#if 0
    if (IsOptionMember("itxt",value) != MagickFalse)
      mng_info->ping_exclude_iTXt=excluding;
#endif

    if (IsOptionMember("offs",value) != MagickFalse)
      mng_info->ping_exclude_oFFs=excluding;

    if (IsOptionMember("phys",value) != MagickFalse)
      mng_info->ping_exclude_pHYs=excluding;

    if (IsOptionMember("srgb",value) != MagickFalse)
      mng_info->ping_exclude_sRGB=excluding;

    if (IsOptionMember("text",value) != MagickFalse)
      mng_info->ping_exclude_tEXt=excluding;

    if (IsOptionMember("time",value) != MagickFalse)
      mng_info->ping_exclude_tIME=excluding;

    if (IsOptionMember("trns",value) != MagickFalse)
      mng_info->ping_exclude_tRNS=excluding;

    if (IsOptionMember("zccp",value) != MagickFalse)
      mng_info->ping_exclude_zCCP=excluding;

    if (IsOptionMember("ztxt",value) != MagickFalse)
      mng_info->ping_exclude_zTXt=excluding;
  }

  if (logging != MagickFalse)
  {
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),
      "  Chunks to be excluded from the output png:");
    if (mng_info->ping_exclude_bKGD != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    bKGD");
    if (mng_info->ping_exclude_caNv != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    caNv");
    if (mng_info->ping_exclude_cHRM != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    cHRM");
    if (mng_info->ping_exclude_date != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    date");
    if (mng_info->ping_exclude_EXIF != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    EXIF");
    if (mng_info->ping_exclude_eXIf != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    eXIf");
    if (mng_info->ping_exclude_gAMA != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    gAMA");
    if (mng_info->ping_exclude_iCCP != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    iCCP");
#if 0
    if (mng_info->ping_exclude_iTXt != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    iTXt");
#endif

    if (mng_info->ping_exclude_oFFs != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    oFFs");
    if (mng_info->ping_exclude_pHYs != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    pHYs");
    if (mng_info->ping_exclude_sRGB != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    sRGB");
    if (mng_info->ping_exclude_tEXt != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    tEXt");
    if (mng_info->ping_exclude_tIME != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    tIME");
    if (mng_info->ping_exclude_tRNS != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    tRNS");
    if (mng_info->ping_exclude_zCCP != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    zCCP");
    if (mng_info->ping_exclude_zTXt != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "    zTXt");
  }

  mng_info->need_blob = MagickTrue;

  status=WriteOnePNGImage(mng_info,image_info,image,exception);

  mng_info=MngInfoFreeStruct(mng_info);

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"exit WritePNGImage()");

  return(status);
}