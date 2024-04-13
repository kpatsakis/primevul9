    //   This is experimental code, not much tested, use with care.
    CImg<T>& load_magick(const char *const filename) {
      if (!filename)
        throw CImgArgumentException(_cimg_instance
                                    "load_magick(): Specified filename is (null).",
                                    cimg_instance);
#ifdef cimg_use_magick
      Magick::Image image(filename);
      const unsigned int W = image.size().width(), H = image.size().height();
      switch (image.type()) {
      case Magick::PaletteMatteType :
      case Magick::TrueColorMatteType :
      case Magick::ColorSeparationType : {
        assign(W,H,1,4);
        T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2), *ptr_a = data(0,0,0,3);
        Magick::PixelPacket *pixels = image.getPixels(0,0,W,H);
        for (ulongT off = (ulongT)W*H; off; --off) {
          *(ptr_r++) = (T)(pixels->red);
          *(ptr_g++) = (T)(pixels->green);
          *(ptr_b++) = (T)(pixels->blue);
          *(ptr_a++) = (T)(pixels->opacity);
          ++pixels;
        }
      } break;
      case Magick::PaletteType :
      case Magick::TrueColorType : {
        assign(W,H,1,3);
        T *ptr_r = data(0,0,0,0), *ptr_g = data(0,0,0,1), *ptr_b = data(0,0,0,2);
        Magick::PixelPacket *pixels = image.getPixels(0,0,W,H);
        for (ulongT off = (ulongT)W*H; off; --off) {
          *(ptr_r++) = (T)(pixels->red);
          *(ptr_g++) = (T)(pixels->green);
          *(ptr_b++) = (T)(pixels->blue);
          ++pixels;
        }
      } break;
      case Magick::GrayscaleMatteType : {
        assign(W,H,1,2);
        T *ptr_r = data(0,0,0,0), *ptr_a = data(0,0,0,1);
        Magick::PixelPacket *pixels = image.getPixels(0,0,W,H);
        for (ulongT off = (ulongT)W*H; off; --off) {
          *(ptr_r++) = (T)(pixels->red);
          *(ptr_a++) = (T)(pixels->opacity);
          ++pixels;
        }
      } break;
      default : {
        assign(W,H,1,1);
        T *ptr_r = data(0,0,0,0);
        Magick::PixelPacket *pixels = image.getPixels(0,0,W,H);
        for (ulongT off = (ulongT)W*H; off; --off) {
          *(ptr_r++) = (T)(pixels->red);
          ++pixels;
        }
      }
      }
      return *this;
#else
      throw CImgIOException(_cimg_instance
                            "load_magick(): Unable to load file '%s' unless libMagick++ is enabled.",
                            cimg_instance,
                            filename);
#endif