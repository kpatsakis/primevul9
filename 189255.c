int LibRaw::raw2image_ex(void)
{
    CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);

    raw2image_start();

    // process cropping
    int do_crop = 0;
    unsigned save_filters = imgdata.idata.filters;
    unsigned save_width = S.width;
    if (~O.cropbox[2] && ~O.cropbox[3])
        {
            int crop[4],c,filt;
            for(int c=0;c<4;c++) 
                {
                    crop[c] = O.cropbox[c];
                    if(crop[c]<0)
                        crop[c]=0;
                }
            if(IO.fwidth) 
                {
                    crop[0] = (crop[0]/4)*4;
                    crop[1] = (crop[1]/4)*4;
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
            if(!IO.fwidth && imgdata.idata.filters)
                {
                    for (filt=c=0; c < 16; c++)
                        filt |= FC((c >> 1)+(crop[1]),
                                   (c &  1)+(crop[0])) << c*2;
                    imgdata.idata.filters = filt;
                }
        }

    if(IO.fwidth) 
        {
            ushort fiwidth,fiheight;
            if(do_crop)
                {
                    IO.fuji_width = S.width >> !libraw_internal_data.unpacker_data.fuji_layout;
                    IO.fwidth = (S.height >> libraw_internal_data.unpacker_data.fuji_layout) + IO.fuji_width;
                    IO.fheight = IO.fwidth - 1;
                }

            fiheight = (IO.fheight + IO.shrink) >> IO.shrink;
            fiwidth = (IO.fwidth + IO.shrink) >> IO.shrink;
            if(imgdata.image)
                    {
                        imgdata.image = (ushort (*)[4])realloc(imgdata.image,fiheight*fiwidth*sizeof (*imgdata.image));
                        memset(imgdata.image,0,fiheight*fiwidth *sizeof (*imgdata.image));
                    }
                else
                    imgdata.image = (ushort (*)[4]) calloc (fiheight*fiwidth, sizeof (*imgdata.image));
            merror (imgdata.image, "raw2image_ex()");

            int cblk[4],i;
            for(i=0;i<4;i++)
                cblk[i] = C.cblack[i]+C.black;
            ZERO(C.channel_maximum);

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
                            
                            int val = imgdata.rawdata.raw_image[(row+S.top_margin)*S.raw_width
                                                            +(col+S.left_margin)];
                            int cc = FCF(row,col);
                            if(val > cblk[cc])
                                val -= cblk[cc];
                            else
                                val = 0;
                            imgdata.image[((r) >> IO.shrink)*fiwidth + ((c) >> IO.shrink)][cc] = val;
                            if(C.channel_maximum[cc] < val) C.channel_maximum[cc] = val;
                        }
                }
            C.maximum -= C.black;
            ZERO(C.cblack);
            C.black = 0;

            // restore fuji sizes!
            S.height = IO.fheight;
            S.width = IO.fwidth;
            S.iheight = (S.height + IO.shrink) >> IO.shrink;
            S.iwidth  = (S.width  + IO.shrink) >> IO.shrink;
            S.raw_height -= 2*S.top_margin;
        }
    else
        {

                if(imgdata.image)
                    {
                        imgdata.image = (ushort (*)[4]) realloc (imgdata.image,S.iheight*S.iwidth 
                                                                 *sizeof (*imgdata.image));
                        memset(imgdata.image,0,S.iheight*S.iwidth *sizeof (*imgdata.image));
                    }
                else
                    imgdata.image = (ushort (*)[4]) calloc (S.iheight*S.iwidth, sizeof (*imgdata.image));

                merror (imgdata.image, "raw2image_ex()");
                
                libraw_decoder_info_t decoder_info;
                get_decoder_info(&decoder_info);


                if(decoder_info.decoder_flags & LIBRAW_DECODER_FLATFIELD)
                    {
                        if(decoder_info.decoder_flags & LIBRAW_DECODER_USEBAYER2)
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for default(shared)
#endif
                            for(int row = 0; row < S.height; row++)
                                for(int col = 0; col < S.width; col++)
                                    imgdata.image[(row >> IO.shrink)*S.iwidth + (col>>IO.shrink)][fc(row,col)]
                                        = imgdata.rawdata.raw_image[(row+S.top_margin)*S.raw_width
                                                                    +(col+S.left_margin)];
                        else
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for default(shared)
#endif
                            for(int row = 0; row < S.height; row++)
                                {
                                    int colors[2];
                                    for (int xx=0;xx<2;xx++)
                                        colors[xx] = COLOR(row,xx);
                                    for(int col = 0; col < S.width; col++)
                                        {
                                            int cc = colors[col&1];
                                            imgdata.image[(row >> IO.shrink)*S.iwidth + (col>>IO.shrink)][cc] =
                                                imgdata.rawdata.raw_image[(row+S.top_margin)*S.raw_width
                                                                          +(col+S.left_margin)];
                                        }
                                }
                    }
                else if (decoder_info.decoder_flags & LIBRAW_DECODER_4COMPONENT)
                    {
#define FC0(row,col) (save_filters >> ((((row) << 1 & 14) + ((col) & 1)) << 1) & 3)
                        if(IO.shrink)
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for default(shared)
#endif
                            for(int row = 0; row < S.height; row++)
                                for(int col = 0; col < S.width; col++)
                                    imgdata.image[(row >> IO.shrink)*S.iwidth + (col>>IO.shrink)][FC(row,col)] 
                                        = imgdata.rawdata.color_image[(row+S.top_margin)*S.raw_width
                                                                      +S.left_margin+col]
                                        [FC0(row+S.top_margin,col+S.left_margin)];
#undef FC0
                        else
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for default(shared)
#endif
                            for(int row = 0; row < S.height; row++)
                                memmove(&imgdata.image[row*S.width],
                                        &imgdata.rawdata.color_image[(row+S.top_margin)*S.raw_width+S.left_margin],
                                        S.width*sizeof(*imgdata.image));
                    }
                else if(decoder_info.decoder_flags & LIBRAW_DECODER_LEGACY)
                    {
                        if(do_crop)
#if defined(LIBRAW_USE_OPENMP)
#pragma omp parallel for default(shared)
#endif
                            for(int row = 0; row < S.height; row++)
                                memmove(&imgdata.image[row*S.width],
                                        &imgdata.rawdata.color_image[(row+S.top_margin)*save_width+S.left_margin],
                                        S.width*sizeof(*imgdata.image));
                                
                        else 
                            memmove(imgdata.image,imgdata.rawdata.color_image,
                                    S.width*S.height*sizeof(*imgdata.image));
                    }

                if(imgdata.rawdata.use_ph1_correct) // Phase one unpacked!
                        phase_one_correct();
            }
    return LIBRAW_SUCCESS;
}