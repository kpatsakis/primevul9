
    CImg<T>& _load_ascii(std::FILE *const file, const char *const filename) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_ascii(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      CImg<charT> line(256); *line = 0;
      int err = std::fscanf(nfile,"%255[^\n]",line._data);
      unsigned int dx = 0, dy = 1, dz = 1, dc = 1;
      cimg_sscanf(line,"%u%*c%u%*c%u%*c%u",&dx,&dy,&dz,&dc);
      err = std::fscanf(nfile,"%*[^0-9.eEinfa+-]");
      if (!dx || !dy || !dz || !dc) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_ascii(): Invalid ascii header in file '%s', image dimensions are set "
                              "to (%u,%u,%u,%u).",
                              cimg_instance,
                              filename?filename:"(FILE*)",dx,dy,dz,dc);
      }
      assign(dx,dy,dz,dc);
      const ulongT siz = size();
      ulongT off = 0;
      double val;
      T *ptr = _data;
      for (err = 1, off = 0; off<siz && err==1; ++off) {
        err = std::fscanf(nfile,"%lf%*[^0-9.eEinfa+-]",&val);
        *(ptr++) = (T)val;
      }
      if (err!=1)
        cimg::warn(_cimg_instance
                   "load_ascii(): Only %lu/%lu values read from file '%s'.",
                   cimg_instance,
                   off - 1,siz,filename?filename:"(FILE*)");

      if (!file) cimg::fclose(nfile);
      return *this;