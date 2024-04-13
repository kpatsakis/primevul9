
    const CImg<T>& _save_pnk(std::FILE *const file, const char *const filename) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_pnk(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }
      if (_spectrum>1)
        cimg::warn(_cimg_instance
                   "save_pnk(): Instance is multispectral, only the first channel will be saved in file '%s'.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      const ulongT buf_size = std::min((ulongT)1024*1024,(ulongT)_width*_height*_depth);
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const T *ptr = data(0,0,0,0);

      if (!cimg::type<T>::is_float() && sizeof(T)==1 && _depth<2) // Can be saved as regular PNM file.
        _save_pnm(file,filename,0);
      else if (!cimg::type<T>::is_float() && sizeof(T)==1) { // Save as extended P5 file: Binary byte-valued 3d.
        std::fprintf(nfile,"P5\n%u %u %u\n255\n",_width,_height,_depth);
        CImg<ucharT> buf((unsigned int)buf_size);
        for (longT to_write = (longT)width()*height()*depth(); to_write>0; ) {
          const ulongT N = std::min((ulongT)to_write,buf_size);
          unsigned char *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) *(ptrd++) = (unsigned char)*(ptr++);
          cimg::fwrite(buf._data,N,nfile);
          to_write-=N;
        }
      } else if (!cimg::type<T>::is_float()) { // Save as P8: Binary int32-valued 3d.
        if (_depth>1) std::fprintf(nfile,"P8\n%u %u %u\n%d\n",_width,_height,_depth,(int)max());
        else std::fprintf(nfile,"P8\n%u %u\n%d\n",_width,_height,(int)max());
        CImg<intT> buf((unsigned int)buf_size);
        for (longT to_write = (longT)width()*height()*depth(); to_write>0; ) {
          const ulongT N = std::min((ulongT)to_write,buf_size);
          int *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) *(ptrd++) = (int)*(ptr++);
          cimg::fwrite(buf._data,N,nfile);
          to_write-=N;
        }
      } else { // Save as P9: Binary float-valued 3d.
        if (_depth>1) std::fprintf(nfile,"P9\n%u %u %u\n%g\n",_width,_height,_depth,(double)max());
        else std::fprintf(nfile,"P9\n%u %u\n%g\n",_width,_height,(double)max());
        CImg<floatT> buf((unsigned int)buf_size);
        for (longT to_write = (longT)width()*height()*depth(); to_write>0; ) {
          const ulongT N = std::min((ulongT)to_write,buf_size);
          float *ptrd = buf._data;
          for (ulongT i = N; i>0; --i) *(ptrd++) = (float)*(ptr++);
          cimg::fwrite(buf._data,N,nfile);
          to_write-=N;
        }
      }

      if (!file) cimg::fclose(nfile);
      return *this;