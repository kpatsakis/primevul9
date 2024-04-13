
    CImg<T>& _load_pandore(std::FILE *const file, const char *const filename) {
#define __cimg_load_pandore_case(nbdim,nwidth,nheight,ndepth,ndim,stype) \
        cimg::fread(dims,nbdim,nfile); \
        if (endian) cimg::invert_endianness(dims,nbdim); \
        assign(nwidth,nheight,ndepth,ndim); \
        const size_t siz = size(); \
        stype *buffer = new stype[siz]; \
        cimg::fread(buffer,siz,nfile); \
        if (endian) cimg::invert_endianness(buffer,siz); \
        T *ptrd = _data; \
        cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++); \
        buffer-=siz; \
        delete[] buffer

#define _cimg_load_pandore_case(nbdim,nwidth,nheight,ndepth,dim,stype1,stype2,stype3,ltype) { \
        if (sizeof(stype1)==ltype) { __cimg_load_pandore_case(nbdim,nwidth,nheight,ndepth,dim,stype1); } \
        else if (sizeof(stype2)==ltype) { __cimg_load_pandore_case(nbdim,nwidth,nheight,ndepth,dim,stype2); } \
        else if (sizeof(stype3)==ltype) { __cimg_load_pandore_case(nbdim,nwidth,nheight,ndepth,dim,stype3); } \
        else throw CImgIOException(_cimg_instance \
                                   "load_pandore(): Unknown pixel datatype in file '%s'.", \
                                   cimg_instance, \
                                   filename?filename:"(FILE*)"); }
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_pandore(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *const nfile = file?file:cimg::fopen(filename,"rb");
      CImg<charT> header(32);
      cimg::fread(header._data,12,nfile);
      if (cimg::strncasecmp("PANDORE",header,7)) {
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pandore(): PANDORE header not found in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      }
      unsigned int imageid, dims[8] = { 0 };
      int ptbuf[4] = { 0 };
      cimg::fread(&imageid,1,nfile);
      const bool endian = imageid>255;
      if (endian) cimg::invert_endianness(imageid);
      cimg::fread(header._data,20,nfile);

      switch (imageid) {
      case 2 : _cimg_load_pandore_case(2,dims[1],1,1,1,unsigned char,unsigned char,unsigned char,1); break;
      case 3 : _cimg_load_pandore_case(2,dims[1],1,1,1,long,int,short,4); break;
      case 4 : _cimg_load_pandore_case(2,dims[1],1,1,1,double,float,float,4); break;
      case 5 : _cimg_load_pandore_case(3,dims[2],dims[1],1,1,unsigned char,unsigned char,unsigned char,1); break;
      case 6 : _cimg_load_pandore_case(3,dims[2],dims[1],1,1,long,int,short,4); break;
      case 7 : _cimg_load_pandore_case(3,dims[2],dims[1],1,1,double,float,float,4); break;
      case 8 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],1,unsigned char,unsigned char,unsigned char,1); break;
      case 9 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],1,long,int,short,4); break;
      case 10 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],1,double,float,float,4); break;
      case 11 : { // Region 1d
        cimg::fread(dims,3,nfile);
        if (endian) cimg::invert_endianness(dims,3);
        assign(dims[1],1,1,1);
        const unsigned siz = size();
        if (dims[2]<256) {
          unsigned char *buffer = new unsigned char[siz];
          cimg::fread(buffer,siz,nfile);
          T *ptrd = _data;
          cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
          buffer-=siz;
          delete[] buffer;
        } else {
          if (dims[2]<65536) {
            unsigned short *buffer = new unsigned short[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          } else {
            unsigned int *buffer = new unsigned int[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          }
        }
      }
        break;
      case 12 : { // Region 2d
        cimg::fread(dims,4,nfile);
        if (endian) cimg::invert_endianness(dims,4);
        assign(dims[2],dims[1],1,1);
        const size_t siz = size();
        if (dims[3]<256) {
          unsigned char *buffer = new unsigned char[siz];
          cimg::fread(buffer,siz,nfile);
          T *ptrd = _data;
          cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
          buffer-=siz;
          delete[] buffer;
        } else {
          if (dims[3]<65536) {
            unsigned short *buffer = new unsigned short[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          } else {
            unsigned int *buffer = new unsigned int[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          }
        }
      }
        break;
      case 13 : { // Region 3d
        cimg::fread(dims,5,nfile);
        if (endian) cimg::invert_endianness(dims,5);
        assign(dims[3],dims[2],dims[1],1);
        const size_t siz = size();
        if (dims[4]<256) {
          unsigned char *buffer = new unsigned char[siz];
          cimg::fread(buffer,siz,nfile);
          T *ptrd = _data;
          cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
          buffer-=siz;
          delete[] buffer;
        } else {
          if (dims[4]<65536) {
            unsigned short *buffer = new unsigned short[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          } else {
            unsigned int *buffer = new unsigned int[siz];
            cimg::fread(buffer,siz,nfile);
            if (endian) cimg::invert_endianness(buffer,siz);
            T *ptrd = _data;
            cimg_foroff(*this,off) *(ptrd++) = (T)*(buffer++);
            buffer-=siz;
            delete[] buffer;
          }
        }
      }
        break;
      case 16 : _cimg_load_pandore_case(4,dims[2],dims[1],1,3,unsigned char,unsigned char,unsigned char,1); break;
      case 17 : _cimg_load_pandore_case(4,dims[2],dims[1],1,3,long,int,short,4); break;
      case 18 : _cimg_load_pandore_case(4,dims[2],dims[1],1,3,double,float,float,4); break;
      case 19 : _cimg_load_pandore_case(5,dims[3],dims[2],dims[1],3,unsigned char,unsigned char,unsigned char,1); break;
      case 20 : _cimg_load_pandore_case(5,dims[3],dims[2],dims[1],3,long,int,short,4); break;
      case 21 : _cimg_load_pandore_case(5,dims[3],dims[2],dims[1],3,double,float,float,4); break;
      case 22 : _cimg_load_pandore_case(2,dims[1],1,1,dims[0],unsigned char,unsigned char,unsigned char,1); break;
      case 23 : _cimg_load_pandore_case(2,dims[1],1,1,dims[0],long,int,short,4); break;
      case 24 : _cimg_load_pandore_case(2,dims[1],1,1,dims[0],unsigned long,unsigned int,unsigned short,4); break;
      case 25 : _cimg_load_pandore_case(2,dims[1],1,1,dims[0],double,float,float,4); break;
      case 26 : _cimg_load_pandore_case(3,dims[2],dims[1],1,dims[0],unsigned char,unsigned char,unsigned char,1); break;
      case 27 : _cimg_load_pandore_case(3,dims[2],dims[1],1,dims[0],long,int,short,4); break;
      case 28 : _cimg_load_pandore_case(3,dims[2],dims[1],1,dims[0],unsigned long,unsigned int,unsigned short,4); break;
      case 29 : _cimg_load_pandore_case(3,dims[2],dims[1],1,dims[0],double,float,float,4); break;
      case 30 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],dims[0],unsigned char,unsigned char,unsigned char,1);
        break;
      case 31 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],dims[0],long,int,short,4); break;
      case 32 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],dims[0],unsigned long,unsigned int,unsigned short,4);
        break;
      case 33 : _cimg_load_pandore_case(4,dims[3],dims[2],dims[1],dims[0],double,float,float,4); break;
      case 34 : { // Points 1d
        cimg::fread(ptbuf,1,nfile);
        if (endian) cimg::invert_endianness(ptbuf,1);
        assign(1); (*this)(0) = (T)ptbuf[0];
      } break;
      case 35 : { // Points 2d
        cimg::fread(ptbuf,2,nfile);
        if (endian) cimg::invert_endianness(ptbuf,2);
        assign(2); (*this)(0) = (T)ptbuf[1]; (*this)(1) = (T)ptbuf[0];
      } break;
      case 36 : { // Points 3d
        cimg::fread(ptbuf,3,nfile);
        if (endian) cimg::invert_endianness(ptbuf,3);
        assign(3); (*this)(0) = (T)ptbuf[2]; (*this)(1) = (T)ptbuf[1]; (*this)(2) = (T)ptbuf[0];
      } break;
      default :
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_pandore(): Unable to load data with ID_type %u in file '%s'.",
                              cimg_instance,
                              imageid,filename?filename:"(FILE*)");
      }
      if (!file) cimg::fclose(nfile);
      return *this;