
    CImg<T>& _load_tiff(TIFF *const tif, const unsigned int directory,
                        float *const voxel_size, CImg<charT> *const description) {
      if (!TIFFSetDirectory(tif,directory)) return assign();
      uint16 samplesperpixel = 1, bitspersample = 8, photo = 0;
      uint16 sampleformat = 1;
      uint32 nx = 1, ny = 1;
      const char *const filename = TIFFFileName(tif);
      const bool is_spp = (bool)TIFFGetField(tif,TIFFTAG_SAMPLESPERPIXEL,&samplesperpixel);
      TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&nx);
      TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&ny);
      TIFFGetField(tif, TIFFTAG_SAMPLEFORMAT, &sampleformat);
      TIFFGetFieldDefaulted(tif,TIFFTAG_BITSPERSAMPLE,&bitspersample);
      TIFFGetField(tif,TIFFTAG_PHOTOMETRIC,&photo);
      if (voxel_size) {
        const char *s_description = 0;
        float vx = 0, vy = 0, vz = 0;
        if (TIFFGetField(tif,TIFFTAG_IMAGEDESCRIPTION,&s_description) && s_description) {
          const char *s_desc = std::strstr(s_description,"VX=");
          if (s_desc && cimg_sscanf(s_desc,"VX=%f VY=%f VZ=%f",&vx,&vy,&vz)==3) { // CImg format.
            voxel_size[0] = vx; voxel_size[1] = vy; voxel_size[2] = vz;
          }
          s_desc = std::strstr(s_description,"spacing=");
          if (s_desc && cimg_sscanf(s_desc,"spacing=%f",&vz)==1) { // fiji format.
            voxel_size[2] = vz;
          }
        }
        TIFFGetField(tif,TIFFTAG_XRESOLUTION,voxel_size);
        TIFFGetField(tif,TIFFTAG_YRESOLUTION,voxel_size + 1);
        voxel_size[0] = 1.0f/voxel_size[0];
        voxel_size[1] = 1.0f/voxel_size[1];
      }
      if (description) {
        const char *s_description = 0;
        if (TIFFGetField(tif,TIFFTAG_IMAGEDESCRIPTION,&s_description) && s_description)
          CImg<charT>::string(s_description).move_to(*description);
      }
      const unsigned int spectrum = !is_spp || photo>=3?(photo>1?3:1):samplesperpixel;
      assign(nx,ny,1,spectrum);

      if ((photo>=3 && sampleformat==1 &&
           (bitspersample==4 || bitspersample==8) &&
           (samplesperpixel==1 || samplesperpixel==3 || samplesperpixel==4)) ||
          (bitspersample==1 && samplesperpixel==1)) {
        // Special case for unsigned color images.
        uint32 *const raster = (uint32*)_TIFFmalloc(nx*ny*sizeof(uint32));
        if (!raster) {
          _TIFFfree(raster); TIFFClose(tif);
          throw CImgException(_cimg_instance
                              "load_tiff(): Failed to allocate memory (%s) for file '%s'.",
                              cimg_instance,
                              cimg::strbuffersize(nx*ny*sizeof(uint32)),filename);
        }
        TIFFReadRGBAImage(tif,nx,ny,raster,0);
        switch (spectrum) {
        case 1 :
          cimg_forXY(*this,x,y)
            (*this)(x,y,0) = (T)(float)TIFFGetR(raster[nx*(ny - 1 -y) + x]);
          break;
        case 3 :
          cimg_forXY(*this,x,y) {
            (*this)(x,y,0) = (T)(float)TIFFGetR(raster[nx*(ny - 1 -y) + x]);
            (*this)(x,y,1) = (T)(float)TIFFGetG(raster[nx*(ny - 1 -y) + x]);
            (*this)(x,y,2) = (T)(float)TIFFGetB(raster[nx*(ny - 1 -y) + x]);
          }
          break;
        case 4 :
          cimg_forXY(*this,x,y) {
            (*this)(x,y,0) = (T)(float)TIFFGetR(raster[nx*(ny - 1 - y) + x]);
            (*this)(x,y,1) = (T)(float)TIFFGetG(raster[nx*(ny - 1 - y) + x]);
            (*this)(x,y,2) = (T)(float)TIFFGetB(raster[nx*(ny - 1 - y) + x]);
            (*this)(x,y,3) = (T)(float)TIFFGetA(raster[nx*(ny - 1 - y) + x]);
          }
          break;
        }
        _TIFFfree(raster);
      } else { // Other cases.
        uint16 config;
        TIFFGetField(tif,TIFFTAG_PLANARCONFIG,&config);
        if (TIFFIsTiled(tif)) {
          uint32 tw = 1, th = 1;
          TIFFGetField(tif,TIFFTAG_TILEWIDTH,&tw);
          TIFFGetField(tif,TIFFTAG_TILELENGTH,&th);
          if (config==PLANARCONFIG_CONTIG) switch (bitspersample) {
            case 8 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_contig<unsigned char>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_contig<signed char>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 16 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_contig<unsigned short>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_contig<short>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 32 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_contig<unsigned int>(tif,samplesperpixel,nx,ny,tw,th);
              else if (sampleformat==SAMPLEFORMAT_INT)
                _load_tiff_tiled_contig<int>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_contig<float>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 64 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_contig<uint64T>(tif,samplesperpixel,nx,ny,tw,th);
              else if (sampleformat==SAMPLEFORMAT_INT)
                _load_tiff_tiled_contig<int64T>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_contig<double>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            } else switch (bitspersample) {
            case 8 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_separate<unsigned char>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_separate<signed char>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 16 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_separate<unsigned short>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_separate<short>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 32 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_separate<unsigned int>(tif,samplesperpixel,nx,ny,tw,th);
              else if (sampleformat==SAMPLEFORMAT_INT)
                _load_tiff_tiled_separate<int>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_separate<float>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            case 64 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_tiled_separate<uint64T>(tif,samplesperpixel,nx,ny,tw,th);
              else if (sampleformat==SAMPLEFORMAT_INT)
                _load_tiff_tiled_separate<int64T>(tif,samplesperpixel,nx,ny,tw,th);
              else _load_tiff_tiled_separate<double>(tif,samplesperpixel,nx,ny,tw,th);
              break;
            }
        } else {
          if (config==PLANARCONFIG_CONTIG) switch (bitspersample) {
            case 8 :
              if (sampleformat==SAMPLEFORMAT_UINT)
                _load_tiff_contig<unsigned char>(tif,samplesperpixel,nx,ny);
              else _load_tiff_contig<signed char>(tif,samplesperpixel,nx,ny);
              break;
            case 16 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_contig<unsigned short>(tif,samplesperpixel,nx,ny);
              else _load_tiff_contig<short>(tif,samplesperpixel,nx,ny);
              break;
            case 32 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_contig<unsigned int>(tif,samplesperpixel,nx,ny);
              else if (sampleformat==SAMPLEFORMAT_INT) _load_tiff_contig<int>(tif,samplesperpixel,nx,ny);
              else _load_tiff_contig<float>(tif,samplesperpixel,nx,ny);
              break;
            case 64 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_contig<uint64T>(tif,samplesperpixel,nx,ny);
              else if (sampleformat==SAMPLEFORMAT_INT) _load_tiff_contig<int64T>(tif,samplesperpixel,nx,ny);
              else _load_tiff_contig<double>(tif,samplesperpixel,nx,ny);
              break;
            } else switch (bitspersample) {
            case 8 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_separate<unsigned char>(tif,samplesperpixel,nx,ny);
              else _load_tiff_separate<signed char>(tif,samplesperpixel,nx,ny);
              break;
            case 16 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_separate<unsigned short>(tif,samplesperpixel,nx,ny);
              else _load_tiff_separate<short>(tif,samplesperpixel,nx,ny);
              break;
            case 32 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_separate<unsigned int>(tif,samplesperpixel,nx,ny);
              else if (sampleformat==SAMPLEFORMAT_INT) _load_tiff_separate<int>(tif,samplesperpixel,nx,ny);
              else _load_tiff_separate<float>(tif,samplesperpixel,nx,ny);
              break;
            case 64 :
              if (sampleformat==SAMPLEFORMAT_UINT) _load_tiff_separate<uint64T>(tif,samplesperpixel,nx,ny);
              else if (sampleformat==SAMPLEFORMAT_INT) _load_tiff_separate<int64T>(tif,samplesperpixel,nx,ny);
              else _load_tiff_separate<double>(tif,samplesperpixel,nx,ny);
              break;
            }
        }
      }
      return *this;