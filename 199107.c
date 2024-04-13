      return CImgList<T>().load_yuv(file,size_x,size_y,chroma_subsampling,
                                    first_frame,last_frame,step_frame,yuv2rgb);
    }

    CImgList<T>& _load_yuv(std::FILE *const file, const char *const filename,
			   const unsigned int size_x, const unsigned int size_y,
                           const unsigned int chroma_subsampling,
			   const unsigned int first_frame, const unsigned int last_frame,
			   const unsigned int step_frame, const bool yuv2rgb) {
      if (!filename && !file)
        throw CImgArgumentException(_cimglist_instance
                                    "load_yuv(): Specified filename is (null).",
                                    cimglist_instance);
      if (chroma_subsampling!=420 && chroma_subsampling!=422 && chroma_subsampling!=444)
        throw CImgArgumentException(_cimglist_instance
                                    "load_yuv(): Specified chroma subsampling '%u' is invalid, for file '%s'.",
                                    cimglist_instance,
                                    chroma_subsampling,filename?filename:"(FILE*)");
      const unsigned int
        cfx = chroma_subsampling==420 || chroma_subsampling==422?2:1,
        cfy = chroma_subsampling==420?2:1,
	nfirst_frame = first_frame<last_frame?first_frame:last_frame,
	nlast_frame = first_frame<last_frame?last_frame:first_frame,
	nstep_frame = step_frame?step_frame:1;

      if (!size_x || !size_y || size_x%cfx || size_y%cfy)
        throw CImgArgumentException(_cimglist_instance
                                    "load_yuv(): Specified dimensions (%u,%u) are invalid, for file '%s'.",
                                    cimglist_instance,
                                    size_x,size_y,filename?filename:"(FILE*)");

      CImg<ucharT> YUV(size_x,size_y,1,3), UV(size_x/cfx,size_y/cfy,1,2);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      bool stop_flag = false;
      int err;
      if (nfirst_frame) {
        err = cimg::fseek(nfile,(uint64T)nfirst_frame*(YUV._width*YUV._height + 2*UV._width*UV._height),SEEK_CUR);
        if (err) {
          if (!file) cimg::fclose(nfile);
          throw CImgIOException(_cimglist_instance
                                "load_yuv(): File '%s' doesn't contain frame number %u.",
                                cimglist_instance,
                                filename?filename:"(FILE*)",nfirst_frame);
        }
      }
      unsigned int frame;
      for (frame = nfirst_frame; !stop_flag && frame<=nlast_frame; frame+=nstep_frame) {
        YUV.get_shared_channel(0).fill(0);
        // *TRY* to read the luminance part, do not replace by cimg::fread!
        err = (int)std::fread((void*)(YUV._data),1,(size_t)YUV._width*YUV._height,nfile);
        if (err!=(int)(YUV._width*YUV._height)) {
          stop_flag = true;
          if (err>0)
            cimg::warn(_cimglist_instance
                       "load_yuv(): File '%s' contains incomplete data or given image dimensions "
                       "(%u,%u) are incorrect.",
                       cimglist_instance,
                       filename?filename:"(FILE*)",size_x,size_y);
        } else {
          UV.fill(0);
          // *TRY* to read the luminance part, do not replace by cimg::fread!
          err = (int)std::fread((void*)(UV._data),1,(size_t)UV.size(),nfile);
          if (err!=(int)(UV.size())) {
            stop_flag = true;
            if (err>0)
              cimg::warn(_cimglist_instance
                         "load_yuv(): File '%s' contains incomplete data or given image dimensions "
                         "(%u,%u) are incorrect.",
                         cimglist_instance,
                         filename?filename:"(FILE*)",size_x,size_y);
          } else {
            const ucharT *ptrs1 = UV._data, *ptrs2 = UV.data(0,0,0,1);
            ucharT *ptrd1 = YUV.data(0,0,0,1), *ptrd2 = YUV.data(0,0,0,2);
            const unsigned int wd = YUV._width;
            switch (chroma_subsampling) {
            case 420 :
              cimg_forY(UV,y) {
                cimg_forX(UV,x) {
                  const ucharT U = *(ptrs1++), V = *(ptrs2++);
                  ptrd1[wd] = U; *(ptrd1)++ = U;
                  ptrd1[wd] = U; *(ptrd1)++ = U;
                  ptrd2[wd] = V; *(ptrd2)++ = V;
                  ptrd2[wd] = V; *(ptrd2)++ = V;
                }
                ptrd1+=wd; ptrd2+=wd;
              }
              break;
            case 422 :
              cimg_forXY(UV,x,y) {
                const ucharT U = *(ptrs1++), V = *(ptrs2++);
                *(ptrd1++) = U; *(ptrd1++) = U;
                *(ptrd2++) = V; *(ptrd2++) = V;
              }
              break;
            default :
              YUV.draw_image(0,0,0,1,UV);
            }
            if (yuv2rgb) YUV.YCbCrtoRGB();
            insert(YUV);
            if (nstep_frame>1) cimg::fseek(nfile,(uint64T)(nstep_frame - 1)*(size_x*size_y + size_x*size_y/2),SEEK_CUR);
          }
        }
      }
      if (stop_flag && nlast_frame!=~0U && frame!=nlast_frame)
        cimg::warn(_cimglist_instance
                   "load_yuv(): Frame %d not reached since only %u frames were found in file '%s'.",
                   cimglist_instance,
                   nlast_frame,frame - 1,filename?filename:"(FILE*)");