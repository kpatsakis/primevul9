int LibRaw::dcraw_document_mode_processing(void)
{
//    CHECK_ORDER_HIGH(LIBRAW_PROGRESS_PRE_INTERPOLATE);
    CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);

    try {

        int no_crop = 1;

        if (~O.cropbox[2] && ~O.cropbox[3])
            no_crop=0;

        raw2image_ex(); // raw2image+crop+rotate_fuji_raw

        if (IO.zero_is_bad)
            {
                remove_zeroes();
                SET_PROC_FLAG(LIBRAW_PROGRESS_REMOVE_ZEROES);
            }

        if(!IO.fuji_width)
            subtract_black();
        
        O.document_mode = 2;
        
        if(P1.is_foveon)
            {
                // filter image data for foveon document mode
                short *iptr = (short *)imgdata.image;
                for (int i=0; i < S.height*S.width*4; i++)
                    {
                        if ((short) iptr[i] < 0) 
                            iptr[i] = 0;
                    }
                SET_PROC_FLAG(LIBRAW_PROGRESS_FOVEON_INTERPOLATE);
            }

        O.use_fuji_rotate = 0;

        if(O.bad_pixels && no_crop) 
            {
                bad_pixels(O.bad_pixels);
                SET_PROC_FLAG(LIBRAW_PROGRESS_BAD_PIXELS);
            }
        if (O.dark_frame && no_crop)
            {
                subtract (O.dark_frame);
                SET_PROC_FLAG(LIBRAW_PROGRESS_DARK_FRAME);
            }


        adjust_maximum();

        if (O.user_sat > 0) 
            C.maximum = O.user_sat;

        pre_interpolate();
        SET_PROC_FLAG(LIBRAW_PROGRESS_PRE_INTERPOLATE);

        if (libraw_internal_data.internal_output_params.mix_green)
            {
                int i;
                for (P1.colors=3, i=0; i < S.height*S.width; i++)
                    imgdata.image[i][1] = (imgdata.image[i][1] + imgdata.image[i][3]) >> 1;
            }
        SET_PROC_FLAG(LIBRAW_PROGRESS_MIX_GREEN);

        if (!P1.is_foveon && P1.colors == 3) 
            median_filter();
        SET_PROC_FLAG(LIBRAW_PROGRESS_MEDIAN_FILTER);

        if (!P1.is_foveon && O.highlight == 2) 
            blend_highlights();

        if (!P1.is_foveon && O.highlight > 2) 
            recover_highlights();
        SET_PROC_FLAG(LIBRAW_PROGRESS_HIGHLIGHTS);

        if (O.use_fuji_rotate) 
            fuji_rotate();
        SET_PROC_FLAG(LIBRAW_PROGRESS_FUJI_ROTATE);
#ifndef NO_LCMS
	if(O.camera_profile)
            {
                apply_profile(O.camera_profile,O.output_profile);
                SET_PROC_FLAG(LIBRAW_PROGRESS_APPLY_PROFILE);
            }
#endif
        if(!libraw_internal_data.output_data.histogram)
            {
                libraw_internal_data.output_data.histogram = (int (*)[LIBRAW_HISTOGRAM_SIZE]) malloc(sizeof(*libraw_internal_data.output_data.histogram)*4);
                merror(libraw_internal_data.output_data.histogram,"LibRaw::dcraw_document_mode_processing()");
            }
        convert_to_rgb();
        SET_PROC_FLAG(LIBRAW_PROGRESS_CONVERT_RGB);

        if (O.use_fuji_rotate)
            stretch();
        SET_PROC_FLAG(LIBRAW_PROGRESS_STRETCH);

        return 0;
    }
    catch ( LibRaw_exceptions err) {
        EXCEPTION_HANDLER(err);
    }

}