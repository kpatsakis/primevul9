    template<typename tc1, typename tc2, typename t>
    CImg<T>& _draw_text(const int x0, const int y0,
                        const char *const text,
                        const tc1 *const foreground_color, const tc2 *const background_color,
                        const float opacity, const CImgList<t>& font,
                        const bool is_native_font) {
      if (!text) return *this;
      if (!font)
        throw CImgArgumentException(_cimg_instance
                                    "draw_text(): Empty specified font.",
                                    cimg_instance);

      const unsigned int text_length = (unsigned int)std::strlen(text);
      const bool _is_empty = is_empty();
      if (_is_empty) {
        // If needed, pre-compute necessary size of the image
        int x = 0, y = 0, w = 0;
        unsigned char c = 0;
        for (unsigned int i = 0; i<text_length; ++i) {
          c = (unsigned char)text[i];
          switch (c) {
          case '\n' : y+=font[0]._height; if (x>w) w = x; x = 0; break;
          case '\t' : x+=4*font[' ']._width; break;
          default : if (c<font._width) x+=font[c]._width;
          }
        }
        if (x!=0 || c=='\n') {
          if (x>w) w=x;
          y+=font[0]._height;
        }
        assign(x0 + w,y0 + y,1,is_native_font?1:font[0]._spectrum,(T)0);
      }

      int x = x0, y = y0;
      for (unsigned int i = 0; i<text_length; ++i) {
        const unsigned char c = (unsigned char)text[i];
        switch (c) {
        case '\n' : y+=font[0]._height; x = x0; break;
        case '\t' : x+=4*font[' ']._width; break;
        default : if (c<font._width) {
            CImg<T> letter = font[c];
            if (letter) {
              if (is_native_font && _spectrum>letter._spectrum) letter.resize(-100,-100,1,_spectrum,0,2);
              const unsigned int cmin = std::min(_spectrum,letter._spectrum);
              if (foreground_color)
                for (unsigned int c = 0; c<cmin; ++c)
                  if (foreground_color[c]!=1) letter.get_shared_channel(c)*=foreground_color[c];
              if (c + 256<font.width()) { // Letter has mask.
                if (background_color)
                  for (unsigned int c = 0; c<cmin; ++c)
                    draw_rectangle(x,y,0,c,x + letter._width - 1,y + letter._height - 1,0,c,
                                   background_color[c],opacity);
                draw_image(x,y,letter,font[c + 256],opacity,255.0f);
              } else draw_image(x,y,letter,opacity); // Letter has no mask.
              x+=letter._width;
            }
          }
        }
      }
      return *this;