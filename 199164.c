
    CImg<T>& _load_pnm(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_pnm(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      unsigned int ppm_type, W, H, D = 1, colormax = 255;
      CImg<charT> item(16384,1,1,1,0);
      int err, rval, gval, bval;
      const longT cimg_iobuffer = (longT)24*1024*1024;
      while ((err=std::fscanf(nfile,"%16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
      if (cimg_sscanf(item," P%u",&ppm_type)!=1) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pnm(): PNM header not found in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      while ((err=std::fscanf(nfile," %16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
      if ((err=cimg_sscanf(item," %u %u %u %u",&W,&H,&D,&colormax))<2) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pnm(): WIDTH and HEIGHT fields undefined in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      if (ppm_type!=1 && ppm_type!=4) {
        if (err==2 || (err==3 && (ppm_type==5 || ppm_type==7 || ppm_type==8 || ppm_type==9))) {
          while ((err=std::fscanf(nfile," %16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
          if (cimg_sscanf(item,"%u",&colormax)!=1)
            cimg::warn(_cimg_instance
                       "load_pnm(): COLORMAX field is undefined in file '%s'.",
                       cimg_instance,
                       filename?filename:"(FILE*)");
        } else { colormax = D; D = 1; }
      }
      std::fgetc(nfile);

      switch (ppm_type) {
      case 1 : { // 2d b&w ascii.
        assign(W,H,1,1);
        T* ptrd = _data;
        cimg_foroff(*this,off) { if (std::fscanf(nfile,"%d",&rval)>0) *(ptrd++) = (T)(rval?0:255); else break; }
      } break;
      case 2 : { // 2d grey ascii.
        assign(W,H,1,1);
        T* ptrd = _data;
        cimg_foroff(*this,off) { if (std::fscanf(nfile,"%d",&rval)>0) *(ptrd++) = (T)rval; else break; }
      } break;
      case 3 : { // 2d color ascii.
        assign(W,H,1,3);
        T *ptrd = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2);
        cimg_forXY(*this,x,y) {
          if (std::fscanf(nfile,"%d %d %d",&rval,&gval,&bval)==3) {
            *(ptrd++) = (T)rval; *(ptr_g++) = (T)gval; *(ptr_b++) = (T)bval;
          } else break;
        }
      } break;
      case 4 : { // 2d b&w binary (support 3D PINK extension).
        CImg<ucharT> raw;
        assign(W,H,D,1);
        T *ptrd = data(0,0,0,0);
        unsigned int w = 0, h = 0, d = 0;
        for (longT to_read = (longT)((W/8 + (W%8?1:0))*H*D); to_read>0; ) {
          raw.assign(std::min(to_read,cimg_iobuffer));
          cimg::fread(raw._data,raw._width,nfile);
          to_read-=raw._width;
          const unsigned char *ptrs = raw._data;
          unsigned char mask = 0, val = 0;
          for (ulongT off = (ulongT)raw._width; off || mask; mask>>=1) {
            if (!mask) { if (off--) val = *(ptrs++); mask = 128; }
            *(ptrd++) = (T)((val&mask)?0:255);
            if (++w==W) { w = 0; mask = 0; if (++h==H) { h = 0; if (++d==D) break; }}
          }
        }
      } break;
      case 5 : case 7 : { // 2d/3d grey binary (support 3D PINK extension).
        if (colormax<256) { // 8 bits.
          CImg<ucharT> raw;
          assign(W,H,D,1);
          T *ptrd = data(0,0,0,0);
          for (longT to_read = (longT)size(); to_read>0; ) {
            raw.assign(std::min(to_read,cimg_iobuffer));
            cimg::fread(raw._data,raw._width,nfile);
            to_read-=raw._width;
            const unsigned char *ptrs = raw._data;
            for (ulongT off = (ulongT)raw._width; off; --off) *(ptrd++) = (T)*(ptrs++);
          }
        } else { // 16 bits.
          CImg<ushortT> raw;
          assign(W,H,D,1);
          T *ptrd = data(0,0,0,0);
          for (longT to_read = (longT)size(); to_read>0; ) {
            raw.assign(std::min(to_read,cimg_iobuffer/2));
            cimg::fread(raw._data,raw._width,nfile);
	    if (!cimg::endianness()) cimg::invert_endianness(raw._data,raw._width);
            to_read-=raw._width;
            const unsigned short *ptrs = raw._data;
            for (ulongT off = (ulongT)raw._width; off; --off) *(ptrd++) = (T)*(ptrs++);
          }
        }
      } break;
      case 6 : { // 2d color binary.
        if (colormax<256) { // 8 bits.
          CImg<ucharT> raw;
          assign(W,H,1,3);
          T
            *ptr_r = data(0,0,0,0),
            *ptr_g = data(0,0,0,1),
            *ptr_b = data(0,0,0,2);
          for (longT to_read = (longT)size(); to_read>0; ) {
            raw.assign(std::min(to_read,cimg_iobuffer));
            cimg::fread(raw._data,raw._width,nfile);
            to_read-=raw._width;
            const unsigned char *ptrs = raw._data;
            for (ulongT off = (ulongT)raw._width/3; off; --off) {
              *(ptr_r++) = (T)*(ptrs++);
              *(ptr_g++) = (T)*(ptrs++);
              *(ptr_b++) = (T)*(ptrs++);
            }
          }
        } else { // 16 bits.
          CImg<ushortT> raw;
          assign(W,H,1,3);
          T
            *ptr_r = data(0,0,0,0),
            *ptr_g = data(0,0,0,1),
            *ptr_b = data(0,0,0,2);
          for (longT to_read = (longT)size(); to_read>0; ) {
            raw.assign(std::min(to_read,cimg_iobuffer/2));
            cimg::fread(raw._data,raw._width,nfile);
            if (!cimg::endianness()) cimg::invert_endianness(raw._data,raw._width);
            to_read-=raw._width;
            const unsigned short *ptrs = raw._data;
            for (ulongT off = (ulongT)raw._width/3; off; --off) {
              *(ptr_r++) = (T)*(ptrs++);
              *(ptr_g++) = (T)*(ptrs++);
              *(ptr_b++) = (T)*(ptrs++);
            }
          }
        }
      } break;
      case 8 : { // 2d/3d grey binary with int32 integers (PINK extension).
        CImg<intT> raw;
        assign(W,H,D,1);
        T *ptrd = data(0,0,0,0);
        for (longT to_read = (longT)size(); to_read>0; ) {
          raw.assign(std::min(to_read,cimg_iobuffer));
          cimg::fread(raw._data,raw._width,nfile);
          to_read-=raw._width;
          const int *ptrs = raw._data;
          for (ulongT off = (ulongT)raw._width; off; --off) *(ptrd++) = (T)*(ptrs++);
        }
      } break;
      case 9 : { // 2d/3d grey binary with float values (PINK extension).
        CImg<floatT> raw;
        assign(W,H,D,1);
        T *ptrd = data(0,0,0,0);
        for (longT to_read = (longT)size(); to_read>0; ) {
          raw.assign(std::min(to_read,cimg_iobuffer));
          cimg::fread(raw._data,raw._width,nfile);
          to_read-=raw._width;
          const float *ptrs = raw._data;
          for (ulongT off = (ulongT)raw._width; off; --off) *(ptrd++) = (T)*(ptrs++);
        }
      } break;
      default :
        assign();
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pnm(): PNM type 'P%d' found, but type is not supported.",
                              cimg_instance,
                              filename?filename:"(FILE*)",ppm_type);
      }
      if (!file) cimg::fclose(nfile);
      return *this;