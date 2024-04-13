    /**
       \param is_compressed tells if zlib compression must be used for serialization
       (this requires 'cimg_use_zlib' been enabled).
    **/
    CImg<ucharT> get_serialize(const bool is_compressed=false) const {
#ifndef cimg_use_zlib
      if (is_compressed)
        cimg::warn(_cimglist_instance
                   "get_serialize(): Unable to compress data unless zlib is enabled, "
                   "storing them uncompressed.",
                   cimglist_instance);
#endif
      CImgList<ucharT> stream;
      CImg<charT> tmpstr(128);
      const char *const ptype = pixel_type(), *const etype = cimg::endianness()?"big":"little";
      if (std::strstr(ptype,"unsigned")==ptype)
        cimg_snprintf(tmpstr,tmpstr._width,"%u unsigned_%s %s_endian\n",_width,ptype + 9,etype);
      else
        cimg_snprintf(tmpstr,tmpstr._width,"%u %s %s_endian\n",_width,ptype,etype);
      CImg<ucharT>::string(tmpstr,false).move_to(stream);
      cimglist_for(*this,l) {
        const CImg<T>& img = _data[l];
        cimg_snprintf(tmpstr,tmpstr._width,"%u %u %u %u",img._width,img._height,img._depth,img._spectrum);
        CImg<ucharT>::string(tmpstr,false).move_to(stream);
        if (img._data) {
          CImg<T> tmp;
          if (cimg::endianness()) { tmp = img; cimg::invert_endianness(tmp._data,tmp.size()); }
          const CImg<T>& ref = cimg::endianness()?tmp:img;
          bool failed_to_compress = true;
          if (is_compressed) {
#ifdef cimg_use_zlib
            const ulongT siz = sizeof(T)*ref.size();
            uLongf csiz = (ulongT)compressBound(siz);
            Bytef *const cbuf = new Bytef[csiz];
            if (compress(cbuf,&csiz,(Bytef*)ref._data,siz))
              cimg::warn(_cimglist_instance
                         "get_serialize(): Failed to save compressed data, saving them uncompressed.",
                         cimglist_instance);
            else {
              cimg_snprintf(tmpstr,tmpstr._width," #%lu\n",csiz);
              CImg<ucharT>::string(tmpstr,false).move_to(stream);
              CImg<ucharT>(cbuf,csiz).move_to(stream);
              delete[] cbuf;
              failed_to_compress = false;
            }
#endif
          }
          if (failed_to_compress) { // Write in a non-compressed way.
            CImg<charT>::string("\n",false).move_to(stream);
            stream.insert(1);
            stream.back().assign((unsigned char*)ref._data,ref.size()*sizeof(T),1,1,1,true);
          }
        } else CImg<charT>::string("\n",false).move_to(stream);