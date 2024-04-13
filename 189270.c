void LibRaw::raw2image_start()
{
        // restore color,sizes and internal data into raw_image fields
        memmove(&imgdata.color,&imgdata.rawdata.color,sizeof(imgdata.color));
        memmove(&imgdata.sizes,&imgdata.rawdata.sizes,sizeof(imgdata.sizes));
        memmove(&imgdata.idata,&imgdata.rawdata.iparams,sizeof(imgdata.idata));
        memmove(&libraw_internal_data.internal_output_params,&imgdata.rawdata.ioparams,sizeof(libraw_internal_data.internal_output_params));

        if (O.user_flip >= 0)
            S.flip = O.user_flip;
        
        switch ((S.flip+3600) % 360) 
            {
            case 270:  S.flip = 5;  break;
            case 180:  S.flip = 3;  break;
            case  90:  S.flip = 6;  break;
            }

        // adjust for half mode!
        IO.shrink = P1.filters && (O.half_size ||
                                   ((O.threshold || O.aber[0] != 1 || O.aber[2] != 1) ));
        
        S.iheight = (S.height + IO.shrink) >> IO.shrink;
        S.iwidth  = (S.width  + IO.shrink) >> IO.shrink;

        if (O.user_black >= 0) 
            C.black = O.user_black;
}