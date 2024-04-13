int LibRaw::raw2image(void)
{

    CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);

    try {
        raw2image_start();

        // free and re-allocate image bitmap
        if(imgdata.image)
            {
                imgdata.image = (ushort (*)[4]) realloc (imgdata.image,S.iheight*S.iwidth *sizeof (*imgdata.image));
                memset(imgdata.image,0,S.iheight*S.iwidth *sizeof (*imgdata.image));
            }
        else
            imgdata.image = (ushort (*)[4]) calloc (S.iheight*S.iwidth, sizeof (*imgdata.image));

        merror (imgdata.image, "raw2image()");

        libraw_decoder_info_t decoder_info;
        get_decoder_info(&decoder_info);
        
        // Move saved bitmap to imgdata.image
        if(decoder_info.decoder_flags & LIBRAW_DECODER_FLATFIELD)
            {
                if(decoder_info.decoder_flags & LIBRAW_DECODER_USEBAYER2)
                    {
                        for(int row = 0; row < S.height; row++)
                            for(int col = 0; col < S.width; col++)
                                imgdata.image[(row >> IO.shrink)*S.iwidth + (col>>IO.shrink)][fc(row,col)]
                                = imgdata.rawdata.raw_image[(row+S.top_margin)*S.raw_width
                                                                           +(col+S.left_margin)];
                    }
                else
                    {
                        for(int row = 0; row < S.height; row++)
                            {
                                int colors[4];
                                for (int xx=0;xx<4;xx++)
                                    colors[xx] = COLOR(row,xx);
                                for(int col = 0; col < S.width; col++)
                                    {
                                        int cc = colors[col&3];
                                        imgdata.image[(row >> IO.shrink)*S.iwidth + (col>>IO.shrink)][cc] =
                                            imgdata.rawdata.raw_image[(row+S.top_margin)*S.raw_width+(col
                                                                                                      +S.left_margin)];
                                    }
                            }
                    }
            }
        else if (decoder_info.decoder_flags & LIBRAW_DECODER_4COMPONENT)
            {
                if(IO.shrink)
                    {
                        for(int row = 0; row < S.height; row++)
                            for(int col = 0; col < S.width; col++)
                                {
                                    int cc = FC(row,col);
                                    imgdata.image[(row >> IO.shrink)*S.iwidth + (col>>IO.shrink)][cc] 
                                        = imgdata.rawdata.color_image[(row+S.top_margin)*S.raw_width
                                                                      +S.left_margin+col][cc];
                                }
                    }
                else
                    for(int row = 0; row < S.height; row++)
                        memmove(&imgdata.image[row*S.width],
                                &imgdata.rawdata.color_image[(row+S.top_margin)*S.raw_width+S.left_margin],
                                S.width*sizeof(*imgdata.image));
            }
        else if(decoder_info.decoder_flags & LIBRAW_DECODER_LEGACY)
            {
                // legacy is always 4channel and not shrinked!
                memmove(imgdata.image,imgdata.rawdata.color_image,S.width*S.height*sizeof(*imgdata.image));
            }

        if(imgdata.rawdata.use_ph1_correct) // Phase one unpacked!
            phase_one_correct();

        // hack - clear later flags!
        imgdata.progress_flags 
            = LIBRAW_PROGRESS_START|LIBRAW_PROGRESS_OPEN
            |LIBRAW_PROGRESS_IDENTIFY|LIBRAW_PROGRESS_SIZE_ADJUST|LIBRAW_PROGRESS_LOAD_RAW;
        return 0;
    }
    catch ( LibRaw_exceptions err) {
        EXCEPTION_HANDLER(err);
    }
}