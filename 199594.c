
    CImg<T>& _load_pfm(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_pfm(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      char pfm_type;
      CImg<charT> item(16384,1,1,1,0);
      int W = 0, H = 0, err = 0;
      double scale = 0;
      while ((err=std::fscanf(nfile,"%16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
      if (cimg_sscanf(item," P%c",&pfm_type)!=1) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pfm(): PFM header not found in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      while ((err=std::fscanf(nfile," %16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
      if ((err=cimg_sscanf(item," %d %d",&W,&H))<2) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pfm(): WIDTH and HEIGHT fields are undefined in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      if (err==2) {
        while ((err=std::fscanf(nfile," %16383[^\n]",item.data()))!=EOF && (*item=='#' || !err)) std::fgetc(nfile);
        if (cimg_sscanf(item,"%lf",&scale)!=1)
          cimg::warn(_cimg_instance
                     "load_pfm(): SCALE field is undefined in file '%s'.",
                     cimg_instance,
                     filename?filename:"(FILE*)");
      }
      std::fgetc(nfile);
      const bool is_color = (pfm_type=='F'), is_inverted = (scale>0)!=cimg::endianness();
      if (is_color) {
        assign(W,H,1,3,(T)0);
        CImg<floatT> buf(3*W);
        T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2);
        cimg_forY(*this,y) {
          cimg::fread(buf._data,3*W,nfile);
          if (is_inverted) cimg::invert_endianness(buf._data,3*W);
          const float *ptrs = buf._data;
          cimg_forX(*this,x) {
            *(ptr_r++) = (T)*(ptrs++);
            *(ptr_g++) = (T)*(ptrs++);
            *(ptr_b++) = (T)*(ptrs++);
          }
        }
      } else {
        assign(W,H,1,1,(T)0);
        CImg<floatT> buf(W);
        T *ptrd = data(0,0,0,0);
        cimg_forY(*this,y) {
          cimg::fread(buf._data,W,nfile);
          if (is_inverted) cimg::invert_endianness(buf._data,W);
          const float *ptrs = buf._data;
          cimg_forX(*this,x) *(ptrd++) = (T)*(ptrs++);
        }
      }
      if (!file) cimg::fclose(nfile);
      return mirror('y');  // Most of the .pfm files are flipped along the y-axis.