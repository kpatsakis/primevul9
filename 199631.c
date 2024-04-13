
    const CImg<T>& _save_png(std::FILE *const file, const char *const filename,
                             const unsigned int bytes_per_pixel=0) const {
      if (!file && !filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_png(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(file,filename); return *this; }

#ifndef cimg_use_png
      cimg::unused(bytes_per_pixel);
      if (!file) return save_other(filename);
      else throw CImgIOException(_cimg_instance
                                 "save_png(): Unable to save data in '(*FILE)' unless libpng is enabled.",
                                 cimg_instance);
#else

#if defined __GNUC__
      const char *volatile nfilename = filename; // Use 'volatile' to avoid (wrong) g++ warning.
      std::FILE *volatile nfile = file?file:cimg::fopen(nfilename,"wb");
      volatile double stmin, stmax = (double)max_min(stmin);
#else
      const char *nfilename = filename;
      std::FILE *nfile = file?file:cimg::fopen(nfilename,"wb");
      double stmin, stmax = (double)max_min(stmin);
#endif

      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_png(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename);

      if (_spectrum>4)
        cimg::warn(_cimg_instance
                   "save_png(): Instance is multispectral, only the three first channels will be saved in file '%s'.",
                   cimg_instance,
                   filename);

      if (stmin<0 || (bytes_per_pixel==1 && stmax>=256) || stmax>=65536)
        cimg::warn(_cimg_instance
                   "save_png(): Instance has pixel values in [%g,%g], probable type overflow in file '%s'.",
                   cimg_instance,
                   filename,stmin,stmax);

      // Setup PNG structures for write
      png_voidp user_error_ptr = 0;
      png_error_ptr user_error_fn = 0, user_warning_fn = 0;
      png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,user_error_ptr, user_error_fn,
                                                    user_warning_fn);
      if (!png_ptr){
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "save_png(): Failed to initialize 'png_ptr' structure when saving file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_infop info_ptr = png_create_info_struct(png_ptr);
      if (!info_ptr) {
        png_destroy_write_struct(&png_ptr,(png_infopp)0);
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "save_png(): Failed to initialize 'info_ptr' structure when saving file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "save_png(): Encountered unknown fatal error in libpng when saving file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_init_io(png_ptr, nfile);

      const int bit_depth = bytes_per_pixel?(bytes_per_pixel*8):(stmax>=256?16:8);

      int color_type;
      switch (spectrum()) {
      case 1 : color_type = PNG_COLOR_TYPE_GRAY; break;
      case 2 : color_type = PNG_COLOR_TYPE_GRAY_ALPHA; break;
      case 3 : color_type = PNG_COLOR_TYPE_RGB; break;
      default : color_type = PNG_COLOR_TYPE_RGB_ALPHA;
      }
      const int interlace_type = PNG_INTERLACE_NONE;
      const int compression_type = PNG_COMPRESSION_TYPE_DEFAULT;
      const int filter_method = PNG_FILTER_TYPE_DEFAULT;
      png_set_IHDR(png_ptr,info_ptr,_width,_height,bit_depth,color_type,interlace_type,compression_type,filter_method);
      png_write_info(png_ptr,info_ptr);
      const int byte_depth = bit_depth>>3;
      const int numChan = spectrum()>4?4:spectrum();
      const int pixel_bit_depth_flag = numChan * (bit_depth - 1);

      // Allocate Memory for Image Save and Fill pixel data
      png_bytep *const imgData = new png_byte*[_height];
      for (unsigned int row = 0; row<_height; ++row) imgData[row] = new png_byte[byte_depth*numChan*_width];
      const T *pC0 = data(0,0,0,0);
      switch (pixel_bit_depth_flag) {
      case 7 :  { // Gray 8-bit
        cimg_forY(*this,y) {
          unsigned char *ptrd = imgData[y];
          cimg_forX(*this,x) *(ptrd++) = (unsigned char)*(pC0++);
        }
      } break;
      case 14 : { // Gray w/ Alpha 8-bit
        const T *pC1 = data(0,0,0,1);
        cimg_forY(*this,y) {
          unsigned char *ptrd = imgData[y];
          cimg_forX(*this,x) {
            *(ptrd++) = (unsigned char)*(pC0++);
            *(ptrd++) = (unsigned char)*(pC1++);
          }
        }
      } break;
      case 21 :  { // RGB 8-bit
        const T *pC1 = data(0,0,0,1), *pC2 = data(0,0,0,2);
        cimg_forY(*this,y) {
          unsigned char *ptrd = imgData[y];
          cimg_forX(*this,x) {
            *(ptrd++) = (unsigned char)*(pC0++);
            *(ptrd++) = (unsigned char)*(pC1++);
            *(ptrd++) = (unsigned char)*(pC2++);
          }
        }
      } break;
      case 28 : { // RGB x/ Alpha 8-bit
        const T *pC1 = data(0,0,0,1), *pC2 = data(0,0,0,2), *pC3 = data(0,0,0,3);
        cimg_forY(*this,y){
          unsigned char *ptrd = imgData[y];
          cimg_forX(*this,x){
            *(ptrd++) = (unsigned char)*(pC0++);
            *(ptrd++) = (unsigned char)*(pC1++);
            *(ptrd++) = (unsigned char)*(pC2++);
            *(ptrd++) = (unsigned char)*(pC3++);
          }
        }
      } break;
      case 15 : { // Gray 16-bit
        cimg_forY(*this,y){
          unsigned short *ptrd = (unsigned short*)(imgData[y]);
          cimg_forX(*this,x) *(ptrd++) = (unsigned short)*(pC0++);
          if (!cimg::endianness()) cimg::invert_endianness((unsigned short*)imgData[y],_width);
        }
      } break;
      case 30 : { // Gray w/ Alpha 16-bit
        const T *pC1 = data(0,0,0,1);
        cimg_forY(*this,y){
          unsigned short *ptrd = (unsigned short*)(imgData[y]);
          cimg_forX(*this,x) {
            *(ptrd++) = (unsigned short)*(pC0++);
            *(ptrd++) = (unsigned short)*(pC1++);
          }
          if (!cimg::endianness()) cimg::invert_endianness((unsigned short*)imgData[y],2*_width);
        }
      } break;
      case 45 : { // RGB 16-bit
        const T *pC1 = data(0,0,0,1), *pC2 = data(0,0,0,2);
        cimg_forY(*this,y) {
          unsigned short *ptrd = (unsigned short*)(imgData[y]);
          cimg_forX(*this,x) {
            *(ptrd++) = (unsigned short)*(pC0++);
            *(ptrd++) = (unsigned short)*(pC1++);
            *(ptrd++) = (unsigned short)*(pC2++);
          }
          if (!cimg::endianness()) cimg::invert_endianness((unsigned short*)imgData[y],3*_width);
        }
      } break;
      case 60 : { // RGB w/ Alpha 16-bit
        const T *pC1 = data(0,0,0,1), *pC2 = data(0,0,0,2), *pC3 = data(0,0,0,3);
        cimg_forY(*this,y) {
          unsigned short *ptrd = (unsigned short*)(imgData[y]);
          cimg_forX(*this,x) {
            *(ptrd++) = (unsigned short)*(pC0++);
            *(ptrd++) = (unsigned short)*(pC1++);
            *(ptrd++) = (unsigned short)*(pC2++);
            *(ptrd++) = (unsigned short)*(pC3++);
          }
          if (!cimg::endianness()) cimg::invert_endianness((unsigned short*)imgData[y],4*_width);
        }
      } break;
      default :
        if (!file) cimg::fclose(nfile);
        throw CImgIOException(_cimg_instance
                              "save_png(): Encountered unknown fatal error in libpng when saving file '%s'.",
                              cimg_instance,
                              nfilename?nfilename:"(FILE*)");
      }
      png_write_image(png_ptr,imgData);
      png_write_end(png_ptr,info_ptr);
      png_destroy_write_struct(&png_ptr, &info_ptr);

      // Deallocate Image Write Memory
      cimg_forY(*this,n) delete[] imgData[n];
      delete[] imgData;

      if (!file) cimg::fclose(nfile);
      return *this;
#endif