
    CImg<T>& _load_raw(std::FILE *const file, const char *const filename,
		       const unsigned int size_x, const unsigned int size_y,
		       const unsigned int size_z, const unsigned int size_c,
		       const bool is_multiplexed, const bool invert_endianness,
                       const ulongT offset) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_raw(): Specified filename is (null).",
                                    cimg_instance);
      if (cimg::is_directory(filename))
        throw CImgArgumentException(_cimg_instance
                                    "load_raw(): Specified filename '%s' is a directory.",
                                    cimg_instance,filename);

      ulongT siz = (ulongT)size_x*size_y*size_z*size_c;
      unsigned int
        _size_x = size_x,
        _size_y = size_y,
        _size_z = size_z,
        _size_c = size_c;
      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      if (!siz) {  // Retrieve file size.
        const longT fpos = cimg::ftell(nfile);
        if (fpos<0) throw CImgArgumentException(_cimg_instance
                                                "load_raw(): Cannot determine size of input file '%s'.",
                                                cimg_instance,filename?filename:"(FILE*)");
        cimg::fseek(nfile,0,SEEK_END);
        siz = cimg::ftell(nfile)/sizeof(T);
		_size_y = (unsigned int)siz;
        _size_x = _size_z = _size_c = 1;
        cimg::fseek(nfile,fpos,SEEK_SET);
      }
      cimg::fseek(nfile,offset,SEEK_SET);
      assign(_size_x,_size_y,_size_z,_size_c,0);
      if (siz && (!is_multiplexed || size_c==1)) {
        cimg::fread(_data,siz,nfile);
        if (invert_endianness) cimg::invert_endianness(_data,siz);
      } else if (siz) {
        CImg<T> buf(1,1,1,_size_c);
        cimg_forXYZ(*this,x,y,z) {
          cimg::fread(buf._data,_size_c,nfile);
          if (invert_endianness) cimg::invert_endianness(buf._data,_size_c);
          set_vector_at(buf,x,y,z);
        }
      }
      if (!file) cimg::fclose(nfile);
      return *this;