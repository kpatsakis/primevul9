int LibRaw::open_datastream(LibRaw_abstract_datastream *stream)
{

    if(!stream)
        return ENOENT;
    if(!stream->valid())
        return LIBRAW_IO_ERROR;
    recycle();

    try {
        ID.input = stream;
        SET_PROC_FLAG(LIBRAW_PROGRESS_OPEN);

        if (O.use_camera_matrix < 0)
            O.use_camera_matrix = O.use_camera_wb;

        identify();

        if(IO.fuji_width)
            {
                IO.fwidth = S.width;
                IO.fheight = S.height;
                S.iwidth = S.width = IO.fuji_width << (int)(!libraw_internal_data.unpacker_data.fuji_layout);
                S.iheight = S.height = S.raw_height;
                S.raw_height += 2*S.top_margin;
            }

        if(C.profile_length)
            {
                if(C.profile) free(C.profile);
                C.profile = malloc(C.profile_length);
                merror(C.profile,"LibRaw::open_file()");
                ID.input->seek(ID.profile_offset,SEEK_SET);
                ID.input->read(C.profile,C.profile_length,1);
            }
        
        SET_PROC_FLAG(LIBRAW_PROGRESS_IDENTIFY);
    }
    catch ( LibRaw_exceptions err) {
        EXCEPTION_HANDLER(err);
    }
    catch (std::exception ee) {
        EXCEPTION_HANDLER(LIBRAW_EXCEPTION_IO_CORRUPT);
    }

    if(P1.raw_count < 1) 
        return LIBRAW_FILE_UNSUPPORTED;

    
    write_fun = &LibRaw::write_ppm_tiff;
    
    if (load_raw == &LibRaw::kodak_ycbcr_load_raw) 
        {
            S.height += S.height & 1;
            S.width  += S.width  & 1;
        }

    IO.shrink = P1.filters && (O.half_size ||
	((O.threshold || O.aber[0] != 1 || O.aber[2] != 1) ));

    S.iheight = (S.height + IO.shrink) >> IO.shrink;
    S.iwidth  = (S.width  + IO.shrink) >> IO.shrink;

    // Save color,sizes and internal data into raw_image fields
    memmove(&imgdata.rawdata.color,&imgdata.color,sizeof(imgdata.color));
    memmove(&imgdata.rawdata.sizes,&imgdata.sizes,sizeof(imgdata.sizes));
    memmove(&imgdata.rawdata.iparams,&imgdata.idata,sizeof(imgdata.idata));
    memmove(&imgdata.rawdata.ioparams,&libraw_internal_data.internal_output_params,sizeof(libraw_internal_data.internal_output_params));
    
    SET_PROC_FLAG(LIBRAW_PROGRESS_SIZE_ADJUST);


    return LIBRAW_SUCCESS;
}