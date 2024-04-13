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

    identify();
    // Adjust sizes for X3F processing
    if(load_raw == &LibRaw::x3f_load_raw)
    {
        for(int i=0; i< foveon_count;i++)
            if(!strcasecmp(imgdata.idata.make,foveon_data[i].make) && !strcasecmp(imgdata.idata.model,foveon_data[i].model)
                && imgdata.sizes.raw_width == foveon_data[i].raw_width
                && imgdata.sizes.raw_height == foveon_data[i].raw_height
                )
            {
                imgdata.sizes.top_margin = foveon_data[i].top_margin;
                imgdata.sizes.left_margin = foveon_data[i].left_margin;
                imgdata.sizes.width = imgdata.sizes.iwidth = foveon_data[i].width;
                imgdata.sizes.height = imgdata.sizes.iheight = foveon_data[i].height;
                break;
            }
    }
#if 0
    size_t bytes = ID.input->size()-libraw_internal_data.unpacker_data.data_offset;
    float bpp = float(bytes)/float(S.raw_width)/float(S.raw_height);
    float bpp2 = float(bytes)/float(S.width)/float(S.height);
    printf("RawSize: %dx%d data offset: %d data size:%d bpp: %g bpp2: %g\n",S.raw_width,S.raw_height,libraw_internal_data.unpacker_data.data_offset,bytes,bpp,bpp2);
    if(!strcasecmp(imgdata.idata.make,"Hasselblad") && bpp == 6.0f)
      {
        load_raw = &LibRaw::hasselblad_full_load_raw;
        S.width = S.raw_width;
        S.height = S.raw_height;
        P1.filters = 0;
        P1.colors=3;
        P1.raw_count=1;
        C.maximum=0xffff;
        printf("3 channel hassy found\n");
      }
#endif
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