static MagickBooleanType WriteMNGImage(const ImageInfo *image_info,
  Image *image, ExceptionInfo *exception)
{
  Image
    *next_image;

  MagickBooleanType
    status;

  volatile MagickBooleanType
    logging;

  MngInfo
    *mng_info;

  int
    image_count,
    need_iterations,
    need_matte;

  volatile int
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_MNG_FEATURES_SUPPORTED)
    need_local_plte,
#endif
    all_images_are_gray,
    need_defi,
    use_global_plte;

  register ssize_t
    i;

  unsigned char
    chunk[800];

  volatile unsigned int
    write_jng,
    write_mng;

  volatile size_t
    scene;

  size_t
    final_delay=0,
    initial_delay;

#if (PNG_LIBPNG_VER < 10200)
    if (image_info->verbose)
      printf("Your PNG library (libpng-%s) is rather old.\n",
         PNG_LIBPNG_VER_STRING);
#endif

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  logging=LogMagickEvent(CoderEvent,GetMagickModule(),"Enter WriteMNGImage()");
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);

  /*
    Allocate a MngInfo structure.
  */
  mng_info=(MngInfo *) AcquireMagickMemory(sizeof(MngInfo));
  if (mng_info == (MngInfo *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  /*
    Initialize members of the MngInfo structure.
  */
  (void) ResetMagickMemory(mng_info,0,sizeof(MngInfo));
  mng_info->image=image;
  write_mng=LocaleCompare(image_info->magick,"MNG") == 0;

  /*
   * See if user has requested a specific PNG subformat to be used
   * for all of the PNGs in the MNG being written, e.g.,
   *
   *    convert *.png png8:animation.mng
   *
   * To do: check -define png:bit_depth and png:color_type as well,
   * or perhaps use mng:bit_depth and mng:color_type instead for
   * global settings.
   */

  mng_info->write_png8=LocaleCompare(image_info->magick,"PNG8") == 0;
  mng_info->write_png24=LocaleCompare(image_info->magick,"PNG24") == 0;
  mng_info->write_png32=LocaleCompare(image_info->magick,"PNG32") == 0;

  write_jng=MagickFalse;
  if (image_info->compression == JPEGCompression)
    write_jng=MagickTrue;

  mng_info->adjoin=image_info->adjoin &&
    (GetNextImageInList(image) != (Image *) NULL) && write_mng;

  if (logging != MagickFalse)
    {
      /* Log some info about the input */
      Image
        *p;

      (void) LogMagickEvent(CoderEvent,GetMagickModule(),
        "  Checking input image(s)\n"
        "    Image_info depth: %.20g,    Type: %d",
        (double) image_info->depth, image_info->type);

      scene=0;
      for (p=image; p != (Image *) NULL; p=GetNextImageInList(p))
      {

        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
           "    Scene: %.20g\n,   Image depth: %.20g",
           (double) scene++, (double) p->depth);

        if (p->alpha_trait != UndefinedPixelTrait)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "      Matte: True");

        else
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "      Matte: False");

        if (p->storage_class == PseudoClass)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "      Storage class: PseudoClass");

        else
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "      Storage class: DirectClass");

        if (p->colors)
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "      Number of colors: %.20g",(double) p->colors);

        else
          (void) LogMagickEvent(CoderEvent,GetMagickModule(),
            "      Number of colors: unspecified");

        if (mng_info->adjoin == MagickFalse)
          break;
      }
    }

  use_global_plte=MagickFalse;
  all_images_are_gray=MagickFalse;
#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
  need_local_plte=MagickTrue;
#endif
  need_defi=MagickFalse;
  need_matte=MagickFalse;
  mng_info->framing_mode=1;
  mng_info->old_framing_mode=1;

  if (write_mng)
      if (image_info->page != (char *) NULL)
        {
          /*
            Determine image bounding box.
          */
          SetGeometry(image,&mng_info->page);
          (void) ParseMetaGeometry(image_info->page,&mng_info->page.x,
            &mng_info->page.y,&mng_info->page.width,&mng_info->page.height);
        }
  if (write_mng)
    {
      unsigned int
        need_geom;

      unsigned short
        red,
        green,
        blue;

      const char *
        option;

      mng_info->page=image->page;
      need_geom=MagickTrue;
      if (mng_info->page.width || mng_info->page.height)
         need_geom=MagickFalse;
      /*
        Check all the scenes.
      */
      initial_delay=image->delay;
      need_iterations=MagickFalse;
      mng_info->equal_chrms=image->chromaticity.red_primary.x != 0.0;
      mng_info->equal_physs=MagickTrue,
      mng_info->equal_gammas=MagickTrue;
      mng_info->equal_srgbs=MagickTrue;
      mng_info->equal_backgrounds=MagickTrue;
      image_count=0;
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_MNG_FEATURES_SUPPORTED)
      all_images_are_gray=MagickTrue;
      mng_info->equal_palettes=MagickFalse;
      need_local_plte=MagickFalse;
#endif
      for (next_image=image; next_image != (Image *) NULL; )
      {
        if (need_geom)
          {
            if ((next_image->columns+next_image->page.x) > mng_info->page.width)
              mng_info->page.width=next_image->columns+next_image->page.x;

            if ((next_image->rows+next_image->page.y) > mng_info->page.height)
              mng_info->page.height=next_image->rows+next_image->page.y;
          }

        if (next_image->page.x || next_image->page.y)
          need_defi=MagickTrue;

        if (next_image->alpha_trait != UndefinedPixelTrait)
          need_matte=MagickTrue;

        if ((int) next_image->dispose >= BackgroundDispose)
          if ((next_image->alpha_trait != UndefinedPixelTrait) ||
               next_image->page.x || next_image->page.y ||
              ((next_image->columns < mng_info->page.width) &&
               (next_image->rows < mng_info->page.height)))
            mng_info->need_fram=MagickTrue;

        if (next_image->iterations)
          need_iterations=MagickTrue;

        final_delay=next_image->delay;

        if (final_delay != initial_delay || final_delay > 1UL*
           next_image->ticks_per_second)
          mng_info->need_fram=1;

#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_MNG_FEATURES_SUPPORTED)
        /*
          check for global palette possibility.
        */
        if (image->alpha_trait != UndefinedPixelTrait)
           need_local_plte=MagickTrue;

        if (need_local_plte == 0)
          {
            if (SetImageGray(image,exception) == MagickFalse)
              all_images_are_gray=MagickFalse;
            mng_info->equal_palettes=PalettesAreEqual(image,next_image);
            if (use_global_plte == 0)
              use_global_plte=mng_info->equal_palettes;
            need_local_plte=!mng_info->equal_palettes;
          }
#endif
        if (GetNextImageInList(next_image) != (Image *) NULL)
          {
            if (next_image->background_color.red !=
                next_image->next->background_color.red ||
                next_image->background_color.green !=
                next_image->next->background_color.green ||
                next_image->background_color.blue !=
                next_image->next->background_color.blue)
              mng_info->equal_backgrounds=MagickFalse;

            if (next_image->gamma != next_image->next->gamma)
              mng_info->equal_gammas=MagickFalse;

            if (next_image->rendering_intent !=
                next_image->next->rendering_intent)
              mng_info->equal_srgbs=MagickFalse;

            if ((next_image->units != next_image->next->units) ||
                (next_image->resolution.x != next_image->next->resolution.x) ||
                (next_image->resolution.y != next_image->next->resolution.y))
              mng_info->equal_physs=MagickFalse;

            if (mng_info->equal_chrms)
              {
                if (next_image->chromaticity.red_primary.x !=
                    next_image->next->chromaticity.red_primary.x ||
                    next_image->chromaticity.red_primary.y !=
                    next_image->next->chromaticity.red_primary.y ||
                    next_image->chromaticity.green_primary.x !=
                    next_image->next->chromaticity.green_primary.x ||
                    next_image->chromaticity.green_primary.y !=
                    next_image->next->chromaticity.green_primary.y ||
                    next_image->chromaticity.blue_primary.x !=
                    next_image->next->chromaticity.blue_primary.x ||
                    next_image->chromaticity.blue_primary.y !=
                    next_image->next->chromaticity.blue_primary.y ||
                    next_image->chromaticity.white_point.x !=
                    next_image->next->chromaticity.white_point.x ||
                    next_image->chromaticity.white_point.y !=
                    next_image->next->chromaticity.white_point.y)
                  mng_info->equal_chrms=MagickFalse;
              }
          }
        image_count++;
        next_image=GetNextImageInList(next_image);
      }
      if (image_count < 2)
        {
          mng_info->equal_backgrounds=MagickFalse;
          mng_info->equal_chrms=MagickFalse;
          mng_info->equal_gammas=MagickFalse;
          mng_info->equal_srgbs=MagickFalse;
          mng_info->equal_physs=MagickFalse;
          use_global_plte=MagickFalse;
#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
          need_local_plte=MagickTrue;
#endif
          need_iterations=MagickFalse;
        }

     if (mng_info->need_fram == MagickFalse)
       {
         /*
           Only certain framing rates 100/n are exactly representable without
           the FRAM chunk but we'll allow some slop in VLC files
         */
         if (final_delay == 0)
           {
             if (need_iterations != MagickFalse)
               {
                 /*
                   It's probably a GIF with loop; don't run it *too* fast.
                 */
                 if (mng_info->adjoin)
                   {
                     final_delay=10;
                     (void) ThrowMagickException(exception,GetMagickModule(),
                       CoderWarning,
                       "input has zero delay between all frames; assuming",
                       " 10 cs `%s'","");
                   }
               }
             else
               mng_info->ticks_per_second=0;
           }
         if (final_delay != 0)
           mng_info->ticks_per_second=(png_uint_32)
              (image->ticks_per_second/final_delay);
         if (final_delay > 50)
           mng_info->ticks_per_second=2;

         if (final_delay > 75)
           mng_info->ticks_per_second=1;

         if (final_delay > 125)
           mng_info->need_fram=MagickTrue;

         if (need_defi && final_delay > 2 && (final_delay != 4) &&
            (final_delay != 5) && (final_delay != 10) && (final_delay != 20) &&
            (final_delay != 25) && (final_delay != 50) &&
            (final_delay != (size_t) image->ticks_per_second))
           mng_info->need_fram=MagickTrue;  /* make it exact; cannot be VLC */
       }

     if (mng_info->need_fram != MagickFalse)
        mng_info->ticks_per_second=image->ticks_per_second;
     /*
        If pseudocolor, we should also check to see if all the
        palettes are identical and write a global PLTE if they are.
        ../glennrp Feb 99.
     */
     /*
        Write the MNG version 1.0 signature and MHDR chunk.
     */
     (void) WriteBlob(image,8,(const unsigned char *) "\212MNG\r\n\032\n");
     (void) WriteBlobMSBULong(image,28L);  /* chunk data length=28 */
     PNGType(chunk,mng_MHDR);
     LogPNGChunk(logging,mng_MHDR,28L);
     PNGLong(chunk+4,(png_uint_32) mng_info->page.width);
     PNGLong(chunk+8,(png_uint_32) mng_info->page.height);
     PNGLong(chunk+12,mng_info->ticks_per_second);
     PNGLong(chunk+16,0L);  /* layer count=unknown */
     PNGLong(chunk+20,0L);  /* frame count=unknown */
     PNGLong(chunk+24,0L);  /* play time=unknown   */
     if (write_jng)
       {
         if (need_matte)
           {
             if (need_defi || mng_info->need_fram || use_global_plte)
               PNGLong(chunk+28,27L);    /* simplicity=LC+JNG */

             else
               PNGLong(chunk+28,25L);    /* simplicity=VLC+JNG */
           }

         else
           {
             if (need_defi || mng_info->need_fram || use_global_plte)
               PNGLong(chunk+28,19L);  /* simplicity=LC+JNG, no transparency */

             else
               PNGLong(chunk+28,17L);  /* simplicity=VLC+JNG, no transparency */
           }
       }

     else
       {
         if (need_matte)
           {
             if (need_defi || mng_info->need_fram || use_global_plte)
               PNGLong(chunk+28,11L);    /* simplicity=LC */

             else
               PNGLong(chunk+28,9L);    /* simplicity=VLC */
           }

         else
           {
             if (need_defi || mng_info->need_fram || use_global_plte)
               PNGLong(chunk+28,3L);    /* simplicity=LC, no transparency */

             else
               PNGLong(chunk+28,1L);    /* simplicity=VLC, no transparency */
           }
       }
     (void) WriteBlob(image,32,chunk);
     (void) WriteBlobMSBULong(image,crc32(0,chunk,32));
     option=GetImageOption(image_info,"mng:need-cacheoff");
     if (option != (const char *) NULL)
       {
         size_t
           length;
         /*
           Write "nEED CACHEOFF" to turn playback caching off for streaming MNG.
         */
         PNGType(chunk,mng_nEED);
         length=CopyMagickString((char *) chunk+4,"CACHEOFF",20);
         (void) WriteBlobMSBULong(image,(size_t) length);
         LogPNGChunk(logging,mng_nEED,(size_t) length);
         length+=4;
         (void) WriteBlob(image,length,chunk);
         (void) WriteBlobMSBULong(image,crc32(0,chunk,(uInt) length));
       }
     if ((GetPreviousImageInList(image) == (Image *) NULL) &&
         (GetNextImageInList(image) != (Image *) NULL) &&
         (image->iterations != 1))
       {
         /*
           Write MNG TERM chunk
         */
         (void) WriteBlobMSBULong(image,10L);  /* data length=10 */
         PNGType(chunk,mng_TERM);
         LogPNGChunk(logging,mng_TERM,10L);
         chunk[4]=3;  /* repeat animation */
         chunk[5]=0;  /* show last frame when done */
         PNGLong(chunk+6,(png_uint_32) (mng_info->ticks_per_second*
            final_delay/MagickMax(image->ticks_per_second,1)));

         if (image->iterations == 0)
           PNGLong(chunk+10,PNG_UINT_31_MAX);

         else
           PNGLong(chunk+10,(png_uint_32) image->iterations);

         if (logging != MagickFalse)
           {
             (void) LogMagickEvent(CoderEvent,GetMagickModule(),
               "     TERM delay: %.20g",(double) (mng_info->ticks_per_second*
              final_delay/MagickMax(image->ticks_per_second,1)));

             if (image->iterations == 0)
               (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "     TERM iterations: %.20g",(double) PNG_UINT_31_MAX);

             else
               (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "     Image iterations: %.20g",(double) image->iterations);
           }
         (void) WriteBlob(image,14,chunk);
         (void) WriteBlobMSBULong(image,crc32(0,chunk,14));
       }
     /*
       To do: check for cHRM+gAMA == sRGB, and write sRGB instead.
     */
     if ((image->colorspace == sRGBColorspace || image->rendering_intent) &&
          mng_info->equal_srgbs)
       {
         /*
           Write MNG sRGB chunk
         */
         (void) WriteBlobMSBULong(image,1L);
         PNGType(chunk,mng_sRGB);
         LogPNGChunk(logging,mng_sRGB,1L);

         if (image->rendering_intent != UndefinedIntent)
           chunk[4]=(unsigned char)
             Magick_RenderingIntent_to_PNG_RenderingIntent(
             (image->rendering_intent));

         else
           chunk[4]=(unsigned char)
             Magick_RenderingIntent_to_PNG_RenderingIntent(
               (PerceptualIntent));

         (void) WriteBlob(image,5,chunk);
         (void) WriteBlobMSBULong(image,crc32(0,chunk,5));
         mng_info->have_write_global_srgb=MagickTrue;
       }

     else
       {
         if (image->gamma && mng_info->equal_gammas)
           {
             /*
                Write MNG gAMA chunk
             */
             (void) WriteBlobMSBULong(image,4L);
             PNGType(chunk,mng_gAMA);
             LogPNGChunk(logging,mng_gAMA,4L);
             PNGLong(chunk+4,(png_uint_32) (100000*image->gamma+0.5));
             (void) WriteBlob(image,8,chunk);
             (void) WriteBlobMSBULong(image,crc32(0,chunk,8));
             mng_info->have_write_global_gama=MagickTrue;
           }
         if (mng_info->equal_chrms)
           {
             PrimaryInfo
               primary;

             /*
                Write MNG cHRM chunk
             */
             (void) WriteBlobMSBULong(image,32L);
             PNGType(chunk,mng_cHRM);
             LogPNGChunk(logging,mng_cHRM,32L);
             primary=image->chromaticity.white_point;
             PNGLong(chunk+4,(png_uint_32) (100000*primary.x+0.5));
             PNGLong(chunk+8,(png_uint_32) (100000*primary.y+0.5));
             primary=image->chromaticity.red_primary;
             PNGLong(chunk+12,(png_uint_32) (100000*primary.x+0.5));
             PNGLong(chunk+16,(png_uint_32) (100000*primary.y+0.5));
             primary=image->chromaticity.green_primary;
             PNGLong(chunk+20,(png_uint_32) (100000*primary.x+0.5));
             PNGLong(chunk+24,(png_uint_32) (100000*primary.y+0.5));
             primary=image->chromaticity.blue_primary;
             PNGLong(chunk+28,(png_uint_32) (100000*primary.x+0.5));
             PNGLong(chunk+32,(png_uint_32) (100000*primary.y+0.5));
             (void) WriteBlob(image,36,chunk);
             (void) WriteBlobMSBULong(image,crc32(0,chunk,36));
             mng_info->have_write_global_chrm=MagickTrue;
           }
       }
     if (image->resolution.x && image->resolution.y && mng_info->equal_physs)
       {
         /*
            Write MNG pHYs chunk
         */
         (void) WriteBlobMSBULong(image,9L);
         PNGType(chunk,mng_pHYs);
         LogPNGChunk(logging,mng_pHYs,9L);

         if (image->units == PixelsPerInchResolution)
           {
             PNGLong(chunk+4,(png_uint_32)
               (image->resolution.x*100.0/2.54+0.5));

             PNGLong(chunk+8,(png_uint_32)
               (image->resolution.y*100.0/2.54+0.5));

             chunk[12]=1;
           }

         else
           {
             if (image->units == PixelsPerCentimeterResolution)
               {
                 PNGLong(chunk+4,(png_uint_32)
                   (image->resolution.x*100.0+0.5));

                 PNGLong(chunk+8,(png_uint_32)
                   (image->resolution.y*100.0+0.5));

                 chunk[12]=1;
               }

             else
               {
                 PNGLong(chunk+4,(png_uint_32) (image->resolution.x+0.5));
                 PNGLong(chunk+8,(png_uint_32) (image->resolution.y+0.5));
                 chunk[12]=0;
               }
           }
         (void) WriteBlob(image,13,chunk);
         (void) WriteBlobMSBULong(image,crc32(0,chunk,13));
       }
     /*
       Write MNG BACK chunk and global bKGD chunk, if the image is transparent
       or does not cover the entire frame.
     */
     if (write_mng && ((image->alpha_trait != UndefinedPixelTrait) ||
         image->page.x > 0 || image->page.y > 0 || (image->page.width &&
         (image->page.width+image->page.x < mng_info->page.width))
         || (image->page.height && (image->page.height+image->page.y
         < mng_info->page.height))))
       {
         (void) WriteBlobMSBULong(image,6L);
         PNGType(chunk,mng_BACK);
         LogPNGChunk(logging,mng_BACK,6L);
         red=ScaleQuantumToShort(image->background_color.red);
         green=ScaleQuantumToShort(image->background_color.green);
         blue=ScaleQuantumToShort(image->background_color.blue);
         PNGShort(chunk+4,red);
         PNGShort(chunk+6,green);
         PNGShort(chunk+8,blue);
         (void) WriteBlob(image,10,chunk);
         (void) WriteBlobMSBULong(image,crc32(0,chunk,10));
         if (mng_info->equal_backgrounds)
           {
             (void) WriteBlobMSBULong(image,6L);
             PNGType(chunk,mng_bKGD);
             LogPNGChunk(logging,mng_bKGD,6L);
             (void) WriteBlob(image,10,chunk);
             (void) WriteBlobMSBULong(image,crc32(0,chunk,10));
           }
       }

#ifdef PNG_WRITE_EMPTY_PLTE_SUPPORTED
     if ((need_local_plte == MagickFalse) &&
         (image->storage_class == PseudoClass) &&
         (all_images_are_gray == MagickFalse))
       {
         size_t
           data_length;

         /*
           Write MNG PLTE chunk
         */
         data_length=3*image->colors;
         (void) WriteBlobMSBULong(image,data_length);
         PNGType(chunk,mng_PLTE);
         LogPNGChunk(logging,mng_PLTE,data_length);

         for (i=0; i < (ssize_t) image->colors; i++)
         {
           chunk[4+i*3]=(unsigned char) (ScaleQuantumToChar(
             image->colormap[i].red) & 0xff);
           chunk[5+i*3]=(unsigned char) (ScaleQuantumToChar(
             image->colormap[i].green) & 0xff);
           chunk[6+i*3]=(unsigned char) (ScaleQuantumToChar(
             image->colormap[i].blue) & 0xff);
         }

         (void) WriteBlob(image,data_length+4,chunk);
         (void) WriteBlobMSBULong(image,crc32(0,chunk,(uInt) (data_length+4)));
         mng_info->have_write_global_plte=MagickTrue;
       }
#endif
    }
  scene=0;
  mng_info->delay=0;
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_MNG_FEATURES_SUPPORTED)
  mng_info->equal_palettes=MagickFalse;
#endif
  do
  {
    if (mng_info->adjoin)
    {
#if defined(PNG_WRITE_EMPTY_PLTE_SUPPORTED) || \
    defined(PNG_MNG_FEATURES_SUPPORTED)
    /*
      If we aren't using a global palette for the entire MNG, check to
      see if we can use one for two or more consecutive images.
    */
    if (need_local_plte && use_global_plte && !all_images_are_gray)
      {
        if (mng_info->IsPalette)
          {
            /*
              When equal_palettes is true, this image has the same palette
              as the previous PseudoClass image
            */
            mng_info->have_write_global_plte=mng_info->equal_palettes;
            mng_info->equal_palettes=PalettesAreEqual(image,image->next);
            if (mng_info->equal_palettes && !mng_info->have_write_global_plte)
              {
                /*
                  Write MNG PLTE chunk
                */
                size_t
                  data_length;

                data_length=3*image->colors;
                (void) WriteBlobMSBULong(image,data_length);
                PNGType(chunk,mng_PLTE);
                LogPNGChunk(logging,mng_PLTE,data_length);

                for (i=0; i < (ssize_t) image->colors; i++)
                {
                  chunk[4+i*3]=ScaleQuantumToChar(image->colormap[i].red);
                  chunk[5+i*3]=ScaleQuantumToChar(image->colormap[i].green);
                  chunk[6+i*3]=ScaleQuantumToChar(image->colormap[i].blue);
                }

                (void) WriteBlob(image,data_length+4,chunk);
                (void) WriteBlobMSBULong(image,crc32(0,chunk,
                   (uInt) (data_length+4)));
                mng_info->have_write_global_plte=MagickTrue;
              }
          }
        else
          mng_info->have_write_global_plte=MagickFalse;
      }
#endif
    if (need_defi)
      {
        ssize_t
          previous_x,
          previous_y;

        if (scene != 0)
          {
            previous_x=mng_info->page.x;
            previous_y=mng_info->page.y;
          }
        else
          {
            previous_x=0;
            previous_y=0;
          }
        mng_info->page=image->page;
        if ((mng_info->page.x !=  previous_x) ||
            (mng_info->page.y != previous_y))
          {
             (void) WriteBlobMSBULong(image,12L);  /* data length=12 */
             PNGType(chunk,mng_DEFI);
             LogPNGChunk(logging,mng_DEFI,12L);
             chunk[4]=0; /* object 0 MSB */
             chunk[5]=0; /* object 0 LSB */
             chunk[6]=0; /* visible  */
             chunk[7]=0; /* abstract */
             PNGLong(chunk+8,(png_uint_32) mng_info->page.x);
             PNGLong(chunk+12,(png_uint_32) mng_info->page.y);
             (void) WriteBlob(image,16,chunk);
             (void) WriteBlobMSBULong(image,crc32(0,chunk,16));
          }
      }
    }

   mng_info->write_mng=write_mng;

   if ((int) image->dispose >= 3)
     mng_info->framing_mode=3;

   if (mng_info->need_fram && mng_info->adjoin &&
       ((image->delay != mng_info->delay) ||
        (mng_info->framing_mode != mng_info->old_framing_mode)))
     {
       if (image->delay == mng_info->delay)
         {
           /*
             Write a MNG FRAM chunk with the new framing mode.
           */
           (void) WriteBlobMSBULong(image,1L);  /* data length=1 */
           PNGType(chunk,mng_FRAM);
           LogPNGChunk(logging,mng_FRAM,1L);
           chunk[4]=(unsigned char) mng_info->framing_mode;
           (void) WriteBlob(image,5,chunk);
           (void) WriteBlobMSBULong(image,crc32(0,chunk,5));
         }
       else
         {
           /*
             Write a MNG FRAM chunk with the delay.
           */
           (void) WriteBlobMSBULong(image,10L);  /* data length=10 */
           PNGType(chunk,mng_FRAM);
           LogPNGChunk(logging,mng_FRAM,10L);
           chunk[4]=(unsigned char) mng_info->framing_mode;
           chunk[5]=0;  /* frame name separator (no name) */
           chunk[6]=2;  /* flag for changing default delay */
           chunk[7]=0;  /* flag for changing frame timeout */
           chunk[8]=0;  /* flag for changing frame clipping */
           chunk[9]=0;  /* flag for changing frame sync_id */
           PNGLong(chunk+10,(png_uint_32)
             ((mng_info->ticks_per_second*
             image->delay)/MagickMax(image->ticks_per_second,1)));
           (void) WriteBlob(image,14,chunk);
           (void) WriteBlobMSBULong(image,crc32(0,chunk,14));
           mng_info->delay=(png_uint_32) image->delay;
         }
       mng_info->old_framing_mode=mng_info->framing_mode;
     }

#if defined(JNG_SUPPORTED)
   if (image_info->compression == JPEGCompression)
     {
       ImageInfo
         *write_info;

       if (logging != MagickFalse)
         (void) LogMagickEvent(CoderEvent,GetMagickModule(),
           "  Writing JNG object.");
       /* To do: specify the desired alpha compression method. */
       write_info=CloneImageInfo(image_info);
       write_info->compression=UndefinedCompression;
       status=WriteOneJNGImage(mng_info,write_info,image,exception);
       write_info=DestroyImageInfo(write_info);
     }
   else
#endif
     {
       if (logging != MagickFalse)
         (void) LogMagickEvent(CoderEvent,GetMagickModule(),
           "  Writing PNG object.");

       mng_info->need_blob = MagickFalse;
       mng_info->ping_preserve_colormap = MagickFalse;

       /* We don't want any ancillary chunks written */
       mng_info->ping_exclude_bKGD=MagickTrue;
       mng_info->ping_exclude_caNv=MagickTrue;
       mng_info->ping_exclude_cHRM=MagickTrue;
       mng_info->ping_exclude_date=MagickTrue;
       mng_info->ping_exclude_EXIF=MagickTrue;
       mng_info->ping_exclude_gAMA=MagickTrue;
       mng_info->ping_exclude_iCCP=MagickTrue;
       /* mng_info->ping_exclude_iTXt=MagickTrue; */
       mng_info->ping_exclude_oFFs=MagickTrue;
       mng_info->ping_exclude_pHYs=MagickTrue;
       mng_info->ping_exclude_sRGB=MagickTrue;
       mng_info->ping_exclude_tEXt=MagickTrue;
       mng_info->ping_exclude_tRNS=MagickTrue;
       mng_info->ping_exclude_zCCP=MagickTrue;
       mng_info->ping_exclude_zTXt=MagickTrue;

       status=WriteOnePNGImage(mng_info,image_info,image,exception);
     }

    if (status == MagickFalse)
      {
        mng_info=MngInfoFreeStruct(mng_info);
        (void) CloseBlob(image);
        return(MagickFalse);
      }
    (void) CatchImageException(image);
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,
      GetImageListLength(image));

    if (status == MagickFalse)
      break;

  } while (mng_info->adjoin);

  if (write_mng)
    {
      while (GetPreviousImageInList(image) != (Image *) NULL)
        image=GetPreviousImageInList(image);
      /*
        Write the MEND chunk.
      */
      (void) WriteBlobMSBULong(image,0x00000000L);
      PNGType(chunk,mng_MEND);
      LogPNGChunk(logging,mng_MEND,0L);
      (void) WriteBlob(image,4,chunk);
      (void) WriteBlobMSBULong(image,crc32(0,chunk,4));
    }
  /*
    Relinquish resources.
  */
  (void) CloseBlob(image);
  mng_info=MngInfoFreeStruct(mng_info);

  if (logging != MagickFalse)
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"exit WriteMNGImage()");

  return(MagickTrue);
}