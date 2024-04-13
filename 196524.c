int LibRaw::raw2image_ex(int do_subtract_black)
{

  CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);

  try {
    raw2image_start();

    // Compressed P1 files with bl data!
    if (is_phaseone_compressed())
      {
        phase_one_allocate_tempbuffer();
        phase_one_subtract_black((ushort*)imgdata.rawdata.raw_alloc,imgdata.rawdata.raw_image);
        phase_one_correct();
      }

    // process cropping
    int do_crop = 0;
    unsigned save_width = S.width;
    if (~O.cropbox[2] && ~O.cropbox[3] 
#ifdef LIBRAW_DEMOSAIC_PACK_GPL2
        && load_raw != &LibRaw::foveon_sd_load_raw
#endif
        ) // Foveon SD to be cropped later
      {
        int crop[4],c,filt;
        for(int c=0;c<4;c++) 
          {
            crop[c] = O.cropbox[c];
            if(crop[c]<0)
              crop[c]=0;
          }
        
        if(IO.fuji_width && imgdata.idata.filters >= 1000) 
          {
            crop[0] = (crop[0]/4)*4;
            crop[1] = (crop[1]/4)*4;
            if(!libraw_internal_data.unpacker_data.fuji_layout)
              {
                crop[2]*=sqrt(2.0);
                crop[3]/=sqrt(2.0);
              }
            crop[2] = (crop[2]/4+1)*4;
            crop[3] = (crop[3]/4+1)*4;
          }
        else if (imgdata.idata.filters == 1)
          {
            crop[0] = (crop[0]/16)*16;
            crop[1] = (crop[1]/16)*16;
          }
        else if(imgdata.idata.filters == LIBRAW_XTRANS)
          {
            crop[0] = (crop[0]/6)*6;
            crop[1] = (crop[1]/6)*6;
          }
        do_crop = 1;
        
        crop[2] = MIN (crop[2], (signed) S.width-crop[0]);
        crop[3] = MIN (crop[3], (signed) S.height-crop[1]);
        if (crop[2] <= 0 || crop[3] <= 0)
          throw LIBRAW_EXCEPTION_BAD_CROP;
        
        // adjust sizes!
        S.left_margin+=crop[0];
        S.top_margin+=crop[1];
        S.width=crop[2];
        S.height=crop[3];
        
        S.iheight = (S.height + IO.shrink) >> IO.shrink;
        S.iwidth  = (S.width  + IO.shrink) >> IO.shrink;
        if(!IO.fuji_width && imgdata.idata.filters && imgdata.idata.filters >= 1000)
          {
            for (filt=c=0; c < 16; c++)
              filt |= FC((c >> 1)+(crop[1]),
                         (c &  1)+(crop[0])) << c*2;
            imgdata.idata.filters = filt;
          }
      }

    int alloc_width = S.iwidth;
    int alloc_height = S.iheight;
    
    if(IO.fuji_width && do_crop)
      {
        int IO_fw = S.width >> !libraw_internal_data.unpacker_data.fuji_layout;
        int t_alloc_width = (S.height >> libraw_internal_data.unpacker_data.fuji_layout) + IO_fw;
        int t_alloc_height = t_alloc_width - 1;
        alloc_height = (t_alloc_height + IO.shrink) >> IO.shrink;
        alloc_width = (t_alloc_width + IO.shrink) >> IO.shrink;
      }
    int alloc_sz = alloc_width*alloc_height;

    if(imgdata.image)
      {
        imgdata.image = (ushort (*)[4]) realloc (imgdata.image,alloc_sz *sizeof (*imgdata.image));
        memset(imgdata.image,0,alloc_sz *sizeof (*imgdata.image));
      }
    else
      imgdata.image = (ushort (*)[4]) calloc (alloc_sz, sizeof (*imgdata.image));
    merror (imgdata.image, "raw2image_ex()");

    libraw_decoder_info_t decoder_info;
    get_decoder_info(&decoder_info);

    // Adjust black levels
    unsigned short cblack[4]={0,0,0,0};
    unsigned short dmax = 0;
    if(do_subtract_black)
      {
        adjust_bl();
        for(int i=0; i< 4; i++)
          cblack[i] = (unsigned short)C.cblack[i];
      }
        
    // Move saved bitmap to imgdata.image
    if(decoder_info.decoder_flags & LIBRAW_DECODER_FLATFIELD)
      {
        if (IO.fuji_width) 
          {
            if(do_crop)
              {
                IO.fuji_width = S.width >> !libraw_internal_data.unpacker_data.fuji_layout;
                int IO_fwidth = (S.height >> libraw_internal_data.unpacker_data.fuji_layout) + IO.fuji_width;
                int IO_fheight = IO_fwidth - 1;
                
                int row,col;
                for(row=0;row<S.height;row++)
                  {
                    for(col=0;col<S.width;col++)
                      {
                        int r,c;
                        if (libraw_internal_data.unpacker_data.fuji_layout) {
                          r = IO.fuji_width - 1 - col + (row >> 1);
                          c = col + ((row+1) >> 1);
                        } else {
                          r = IO.fuji_width - 1 + row - (col >> 1);
                          c = row + ((col+1) >> 1);
                        }
                        
                        unsigned short val = imgdata.rawdata.raw_image[(row+S.top_margin)*S.raw_pitch/2
                                                            +(col+S.left_margin)];
                        int cc = FCF(row,col);
                        if(val > cblack[cc])
                          {
                            val-=cblack[cc];
                            if(dmax < val) dmax = val;
                          }
                        else
                          val = 0;
                        imgdata.image[((r) >> IO.shrink)*alloc_width + ((c) >> IO.shrink)][cc] = val;
                      }
                  }
                S.height = IO_fheight;
                S.width = IO_fwidth;
                S.iheight = (S.height + IO.shrink) >> IO.shrink;
                S.iwidth  = (S.width  + IO.shrink) >> IO.shrink;
                S.raw_height -= 2*S.top_margin;
              }
            else
              {
                copy_fuji_uncropped(cblack,&dmax);
              }
          } // end Fuji
        else 
          {
            copy_bayer(cblack,&dmax);
          }
      }
    else if(decoder_info.decoder_flags & LIBRAW_DECODER_LEGACY)
      {
        if(imgdata.rawdata.color4_image)
          {
            if(S.raw_pitch != S.width*8)
              {
                for(int row = 0; row < S.height; row++)
                  memmove(&imgdata.image[row*S.width],
                          &imgdata.rawdata.color4_image[(row+S.top_margin)*S.raw_pitch/8+S.left_margin],
                          S.width*sizeof(*imgdata.image));
              }
            else
              {
                // legacy is always 4channel and not shrinked!
                memmove(imgdata.image,imgdata.rawdata.color4_image,S.width*S.height*sizeof(*imgdata.image));
              }
          }
        else if(imgdata.rawdata.color3_image)
          {
            unsigned char *c3image = (unsigned char*) imgdata.rawdata.color3_image;
            for(int row = 0; row < S.height; row++)
              {
                ushort (*srcrow)[3] = (ushort (*)[3]) &c3image[(row+S.top_margin)*S.raw_pitch];
                ushort (*dstrow)[4] = (ushort (*)[4]) &imgdata.image[row*S.width];
                for(int col=0; col < S.width; col++)
                  {
                    for(int c=0; c< 3; c++)
                      dstrow[col][c] = srcrow[S.left_margin+col][c];
                    dstrow[col][3]=0;
                  }
              }
          }
        else
          {
            // legacy decoder, but no data?
            throw LIBRAW_EXCEPTION_DECODE_RAW;
          }
      }

    // Free PhaseOne separate copy allocated at function start
    if (is_phaseone_compressed())
      {
		  phase_one_free_tempbuffer();
      }
    if (load_raw == &CLASS canon_600_load_raw && S.width < S.raw_width) 
      {
        canon_600_correct();
      }

    if(do_subtract_black)
      {
        C.data_maximum = (int)dmax;
        C.maximum -= C.black;
        ZERO(C.cblack);
        C.black = 0;
      }

    // hack - clear later flags!
    imgdata.progress_flags 
      = LIBRAW_PROGRESS_START|LIBRAW_PROGRESS_OPEN | LIBRAW_PROGRESS_RAW2_IMAGE
      |LIBRAW_PROGRESS_IDENTIFY|LIBRAW_PROGRESS_SIZE_ADJUST|LIBRAW_PROGRESS_LOAD_RAW;
    return 0;
  }
  catch ( LibRaw_exceptions err) {
    EXCEPTION_HANDLER(err);
  }
}