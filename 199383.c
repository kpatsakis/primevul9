
    CImg<T>& _load_analyze(std::FILE *const file, const char *const filename, float *const voxel_size=0) {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_analyze(): Specified filename is (null).",
                                    cimg_instance);

      std::FILE *nfile_header = 0, *nfile = 0;
      if (!file) {
        CImg<charT> body(1024);
        const char *const ext = cimg::split_filename(filename,body);
        if (!cimg::strcasecmp(ext,"hdr")) { // File is an Analyze header file.
          nfile_header = cimg::fopen(filename,"rb");
          cimg_sprintf(body._data + std::strlen(body),".img");
          nfile = cimg::fopen(body,"rb");
        } else if (!cimg::strcasecmp(ext,"img")) { // File is an Analyze data file.
          nfile = cimg::fopen(filename,"rb");
          cimg_sprintf(body._data + std::strlen(body),".hdr");
          nfile_header = cimg::fopen(body,"rb");
        } else nfile_header = nfile = cimg::fopen(filename,"rb"); // File is a Niftii file.
      } else nfile_header = nfile = file; // File is a Niftii file.
      if (!nfile || !nfile_header)
        throw CImgIOException(_cimg_instance
                              "load_analyze(): Invalid Analyze7.5 or NIFTI header in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");

      // Read header.
      bool endian = false;
      unsigned int header_size;
      cimg::fread(&header_size,1,nfile_header);
      if (!header_size)
        throw CImgIOException(_cimg_instance
                              "load_analyze(): Invalid zero-size header in file '%s'.",
                              cimg_instance,
                              filename?filename:"(FILE*)");
      if (header_size>=4096) { endian = true; cimg::invert_endianness(header_size); }

      unsigned char *const header = new unsigned char[header_size];
      cimg::fread(header + 4,header_size - 4,nfile_header);
      if (!file && nfile_header!=nfile) cimg::fclose(nfile_header);
      if (endian) {
        cimg::invert_endianness((short*)(header + 40),5);
        cimg::invert_endianness((short*)(header + 70),1);
        cimg::invert_endianness((short*)(header + 72),1);
        cimg::invert_endianness((float*)(header + 76),4);
        cimg::invert_endianness((float*)(header + 108),1);
        cimg::invert_endianness((float*)(header + 112),1);
      }

      if (nfile_header==nfile) {
        const unsigned int vox_offset = (unsigned int)*(float*)(header + 108);
        std::fseek(nfile,vox_offset,SEEK_SET);
      }

      unsigned short *dim = (unsigned short*)(header + 40), dimx = 1, dimy = 1, dimz = 1, dimv = 1;
      if (!dim[0])
        cimg::warn(_cimg_instance
                   "load_analyze(): File '%s' defines an image with zero dimensions.",
                   cimg_instance,
                   filename?filename:"(FILE*)");

      if (dim[0]>4)
        cimg::warn(_cimg_instance
                   "load_analyze(): File '%s' defines an image with %u dimensions, reading only the 4 first.",
                   cimg_instance,
                   filename?filename:"(FILE*)",dim[0]);

      if (dim[0]>=1) dimx = dim[1];
      if (dim[0]>=2) dimy = dim[2];
      if (dim[0]>=3) dimz = dim[3];
      if (dim[0]>=4) dimv = dim[4];
      float scalefactor = *(float*)(header + 112); if (scalefactor==0) scalefactor = 1;
      const unsigned short datatype = *(unsigned short*)(header + 70);
      if (voxel_size) {
        const float *vsize = (float*)(header + 76);
        voxel_size[0] = vsize[1]; voxel_size[1] = vsize[2]; voxel_size[2] = vsize[3];
      }
      delete[] header;

      // Read pixel data.
      assign(dimx,dimy,dimz,dimv);
      const size_t pdim = (size_t)dimx*dimy*dimz*dimv;
      switch (datatype) {
      case 2 : {
        unsigned char *const buffer = new unsigned char[pdim];
        cimg::fread(buffer,pdim,nfile);
        cimg_foroff(*this,off) _data[off] = (T)(buffer[off]*scalefactor);
        delete[] buffer;
      } break;
      case 4 : {
        short *const buffer = new short[pdim];
        cimg::fread(buffer,pdim,nfile);
        if (endian) cimg::invert_endianness(buffer,pdim);
        cimg_foroff(*this,off) _data[off] = (T)(buffer[off]*scalefactor);
        delete[] buffer;
      } break;
      case 8 : {
        int *const buffer = new int[pdim];
        cimg::fread(buffer,pdim,nfile);
        if (endian) cimg::invert_endianness(buffer,pdim);
        cimg_foroff(*this,off) _data[off] = (T)(buffer[off]*scalefactor);
        delete[] buffer;
      } break;
      case 16 : {
        float *const buffer = new float[pdim];
        cimg::fread(buffer,pdim,nfile);
        if (endian) cimg::invert_endianness(buffer,pdim);
        cimg_foroff(*this,off) _data[off] = (T)(buffer[off]*scalefactor);
        delete[] buffer;
      } break;
      case 64 : {
        double *const buffer = new double[pdim];
        cimg::fread(buffer,pdim,nfile);
        if (endian) cimg::invert_endianness(buffer,pdim);
        cimg_foroff(*this,off) _data[off] = (T)(buffer[off]*scalefactor);
        delete[] buffer;
      } break;
      default :
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "load_analyze(): Unable to load datatype %d in file '%s'",
                              cimg_instance,
                              datatype,filename?filename:"(FILE*)");
      }
      if (!file) cimg::fclose(nfile);
      return *this;