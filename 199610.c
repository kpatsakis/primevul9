    **/
    const CImg<T>& save_exr(const char *const filename) const {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "save_exr(): Specified filename is (null).",
                                    cimg_instance);
      if (is_empty()) { cimg::fempty(0,filename); return *this; }
      if (_depth>1)
        cimg::warn(_cimg_instance
                   "save_exr(): Instance is volumetric, only the first slice will be saved in file '%s'.",
                   cimg_instance,
                   filename);

#ifndef cimg_use_openexr
      return save_other(filename);
#else
      Imf::Rgba *const ptrd0 = new Imf::Rgba[(size_t)_width*_height], *ptrd = ptrd0, rgba;
      switch (_spectrum) {
      case 1 : { // Grayscale image.
        for (const T *ptr_r = data(), *const ptr_e = ptr_r + (ulongT)_width*_height; ptr_r<ptr_e;) {
          rgba.r = rgba.g = rgba.b = (half)(*(ptr_r++));
          rgba.a = (half)1;
          *(ptrd++) = rgba;
        }
      } break;
      case 2 : { // RG image.
        for (const T *ptr_r = data(), *ptr_g = data(0,0,0,1),
               *const ptr_e = ptr_r + (ulongT)_width*_height; ptr_r<ptr_e; ) {
          rgba.r = (half)(*(ptr_r++));
          rgba.g = (half)(*(ptr_g++));
          rgba.b = (half)0;
          rgba.a = (half)1;
          *(ptrd++) = rgba;
        }
      } break;
      case 3 : { // RGB image.
        for (const T *ptr_r = data(), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2),
               *const ptr_e = ptr_r + (ulongT)_width*_height; ptr_r<ptr_e;) {
          rgba.r = (half)(*(ptr_r++));
          rgba.g = (half)(*(ptr_g++));
          rgba.b = (half)(*(ptr_b++));
          rgba.a = (half)1;
          *(ptrd++) = rgba;
        }
      } break;
      default : { // RGBA image.
        for (const T *ptr_r = data(), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2), *ptr_a = data(0,0,0,3),
               *const ptr_e = ptr_r + (ulongT)_width*_height; ptr_r<ptr_e;) {
          rgba.r = (half)(*(ptr_r++));
          rgba.g = (half)(*(ptr_g++));
          rgba.b = (half)(*(ptr_b++));
          rgba.a = (half)(*(ptr_a++));
          *(ptrd++) = rgba;
        }
      } break;
      }
      Imf::RgbaOutputFile outFile(filename,_width,_height,
                                  _spectrum==1?Imf::WRITE_Y:_spectrum==2?Imf::WRITE_YA:_spectrum==3?
                                  Imf::WRITE_RGB:Imf::WRITE_RGBA);
      outFile.setFrameBuffer(ptrd0,1,_width);
      outFile.writePixels(_height);
      delete[] ptrd0;
      return *this;
#endif