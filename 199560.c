    const CImgList<T>& save_cimg(const char *const filename, const bool is_compressed=false) const {
      return _save_cimg(0,filename,is_compressed);
    }

    const CImgList<T>& _save_cimg(std::FILE *const file, const char *const filename, const bool is_compressed) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimglist_instance
                                    "save_cimg(): Specified filename is (null).",
                                    cimglist_instance);
#ifndef cimg_use_zlib
      if (is_compressed)
        cimg::warn(_cimglist_instance
                   "save_cimg(): Unable to save compressed data in file '%s' unless zlib is enabled, "
                   "saving them uncompressed.",
                   cimglist_instance,
                   filename?filename:"(FILE*)");
#endif
      std::FILE *const nfile = file?file:cimg::fopen(filename,"wb");
      const char *const ptype = pixel_type(), *const etype = cimg::endianness()?"big":"little";
      if (std::strstr(ptype,"unsigned")==ptype) std::fprintf(nfile,"%u unsigned_%s %s_endian\n",_width,ptype + 9,etype);
      else std::fprintf(nfile,"%u %s %s_endian\n",_width,ptype,etype);
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        std::fprintf(nfile,"%u %u %u %u",img._width,img._height,img._depth,img._spectrum);
        if (img._data) {
          CImg<T> tmp;
          if (cimg::endianness()) { tmp = img; cimg::invert_endianness(tmp._data,tmp.size()); }
          const CImg<T>& ref = cimg::endianness()?tmp:img;
          bool failed_to_compress = true;
          if (is_compressed) {
#ifdef cimg_use_zlib
            const ulongT siz = sizeof(T)*ref.size();
            uLongf csiz = siz + siz/100 + 16;
            Bytef *const cbuf = new Bytef[csiz];
            if (compress(cbuf,&csiz,(Bytef*)ref._data,siz))
              cimg::warn(_cimglist_instance
                         "save_cimg(): Failed to save compressed data for file '%s', saving them uncompressed.",
                         cimglist_instance,
                         filename?filename:"(FILE*)");
            else {
              std::fprintf(nfile," #%lu\n",csiz);
              cimg::fwrite(cbuf,csiz,nfile);
              delete[] cbuf;
              failed_to_compress = false;
            }
#endif
          }
          if (failed_to_compress) { // Write in a non-compressed way.
            std::fputc('\n',nfile);
            cimg::fwrite(ref._data,ref.size(),nfile);
          }
        } else std::fputc('\n',nfile);