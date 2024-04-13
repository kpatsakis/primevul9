    }

    //! Unserialize a CImg<unsigned char> serialized buffer into a CImgList<T> list.
    template<typename t>
    static CImgList<T> get_unserialize(const CImg<t>& buffer) {
#ifdef cimg_use_zlib
#define _cimgz_unserialize_case(Tss) { \
        Bytef *cbuf = 0; \
        if (sizeof(t)!=1 || cimg::type<t>::string()==cimg::type<bool>::string()) { \
          cbuf = new Bytef[csiz]; Bytef *_cbuf = cbuf; \
          for (ulongT i = 0; i<csiz; ++i) *(_cbuf++) = (Bytef)*(stream++); \
          is_bytef = false; \
        } else { cbuf = (Bytef*)stream; stream+=csiz; is_bytef = true; } \
        raw.assign(W,H,D,C); \
        uLongf destlen = raw.size()*sizeof(Tss); \
        uncompress((Bytef*)raw._data,&destlen,cbuf,csiz); \
        if (!is_bytef) delete[] cbuf; \
      }
#else
#define _cimgz_unserialize_case(Tss) \
      throw CImgArgumentException("CImgList<%s>::get_unserialize(): Unable to unserialize compressed data " \
                                  "unless zlib is enabled.", \
                                  pixel_type());
#endif

#define _cimg_unserialize_case(Ts,Tss) \
      if (!loaded && !cimg::strcasecmp(Ts,str_pixeltype)) { \
        for (unsigned int l = 0; l<N; ++l) { \
          j = 0; while ((i=(int)*stream)!='\n' && stream<estream && j<255) { ++stream; tmp[j++] = (char)i; } \
          ++stream; tmp[j] = 0; \
          W = H = D = C = 0; csiz = 0; \
          if ((err = cimg_sscanf(tmp,"%u %u %u %u #" cimg_fuint64,&W,&H,&D,&C,&csiz))<4) \
            throw CImgArgumentException("CImgList<%s>::unserialize(): Invalid specified size (%u,%u,%u,%u) for " \
                                        "image #%u in serialized buffer.", \
                                        pixel_type(),W,H,D,C,l); \
          if (W*H*D*C>0) { \
            CImg<Tss> raw; \
            CImg<T> &img = res._data[l]; \
            if (err==5) _cimgz_unserialize_case(Tss) \
            else if (sizeof(Tss)==sizeof(t) && cimg::type<Tss>::is_float()==cimg::type<t>::is_float()) { \
              raw.assign((Tss*)stream,W,H,D,C,true); \
              stream+=raw.size(); \
            } else { \
              raw.assign(W,H,D,C); \
              CImg<ucharT> _raw((unsigned char*)raw._data,W*sizeof(Tss),H,D,C,true); \
              cimg_for(_raw,p,unsigned char) *p = (unsigned char)*(stream++); \
            } \
            if (endian!=cimg::endianness()) cimg::invert_endianness(raw._data,raw.size()); \
            raw.move_to(img); \
          } \
        } \
        loaded = true; \
      }

      if (buffer.is_empty())
        throw CImgArgumentException("CImgList<%s>::get_unserialize(): Specified serialized buffer is (null).",
                                    pixel_type());
      CImgList<T> res;
      const t *stream = buffer._data, *const estream = buffer._data + buffer.size();
      bool loaded = false, endian = cimg::endianness(), is_bytef = false;
      CImg<charT> tmp(256), str_pixeltype(256), str_endian(256);
      *tmp = *str_pixeltype = *str_endian = 0;
      unsigned int j, N = 0, W, H, D, C;
      uint64T csiz;
      int i, err;
      cimg::unused(is_bytef);
      do {
        j = 0; while ((i=(int)*stream)!='\n' && stream<estream && j<255) { ++stream; tmp[j++] = (char)i; }
        ++stream; tmp[j] = 0;
      } while (*tmp=='#' && stream<estream);
      err = cimg_sscanf(tmp,"%u%*c%255[A-Za-z64_]%*c%255[sA-Za-z_ ]",
                        &N,str_pixeltype._data,str_endian._data);
      if (err<2)
        throw CImgArgumentException("CImgList<%s>::get_unserialize(): CImg header not found in serialized buffer.",
                                    pixel_type());
      if (!cimg::strncasecmp("little",str_endian,6)) endian = false;
      else if (!cimg::strncasecmp("big",str_endian,3)) endian = true;
      res.assign(N);
      _cimg_unserialize_case("bool",bool);
      _cimg_unserialize_case("unsigned_char",unsigned char);
      _cimg_unserialize_case("uchar",unsigned char);
      _cimg_unserialize_case("char",char);
      _cimg_unserialize_case("unsigned_short",unsigned short);
      _cimg_unserialize_case("ushort",unsigned short);
      _cimg_unserialize_case("short",short);
      _cimg_unserialize_case("unsigned_int",unsigned int);
      _cimg_unserialize_case("uint",unsigned int);
      _cimg_unserialize_case("int",int);
      _cimg_unserialize_case("unsigned_int64",uint64T);
      _cimg_unserialize_case("uint64",uint64T);
      _cimg_unserialize_case("int64",int64T);
      _cimg_unserialize_case("float",float);
      _cimg_unserialize_case("double",double);
      if (!loaded)
        throw CImgArgumentException("CImgList<%s>::get_unserialize(): Unsupported pixel type '%s' defined "