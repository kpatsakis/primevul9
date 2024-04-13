    /**
       \param font_height Height of the desired font (exact match for 13,23,53,103).
       \param is_variable_width Decide if the font has a variable (\c true) or fixed (\c false) width.
    **/
    static const CImgList<ucharT>& font(const unsigned int font_height, const bool is_variable_width=true) {
      if (!font_height) return CImgList<ucharT>::const_empty();
      cimg::mutex(11);

      // Decompress nearest base font data if needed.
      static const char *data_fonts[] = { cimg::data_font12x13, cimg::data_font20x23, cimg::data_font47x53, 0 };
      static const unsigned int data_widths[] = { 12,20,47,90 }, data_heights[] = { 13,23,53,103 },
                                data_Ms[] = { 86,79,57,47 };
      const unsigned int data_ind = font_height<=13U?0U:font_height<=23U?1U:font_height<=53U?2U:3U;
      static CImg<ucharT> base_fonts[4];
      CImg<ucharT> &base_font = base_fonts[data_ind];
      if (!base_font) {
        const unsigned int w = data_widths[data_ind], h = data_heights[data_ind], M = data_Ms[data_ind];
        base_font.assign(256*w,h);
        const char *data_font = data_fonts[data_ind];
        unsigned char *ptrd = base_font;
        const unsigned char *const ptrde = base_font.end();

        // Special case needed for 90x103 to avoid MS compiler limit with big strings.
        CImg<char> data90x103;
        if (!data_font) {
          ((CImg<char>(cimg::_data_font90x103[0],
                       (unsigned int)std::strlen(cimg::_data_font90x103[0]),1,1,1,true),
            CImg<char>(cimg::_data_font90x103[1],
                       (unsigned int)std::strlen(cimg::_data_font90x103[1]) + 1,1,1,1,true))>'x').
            move_to(data90x103);
          data_font = data90x103.data();
        }

        // Uncompress font data (decode RLE).
        for (const char *ptrs = data_font; *ptrs; ++ptrs) {
          const int c = (int)(*ptrs - M - 32), v = c>=0?255:0, n = c>=0?c:-c;
          if (ptrd + n<=ptrde) { std::memset(ptrd,v,n); ptrd+=n; }
          else { std::memset(ptrd,v,ptrde - ptrd); break; }
        }
      }

      // Find optimal font cache location to return.
      static CImgList<ucharT> fonts[16];
      static bool is_variable_widths[16] = { 0 };
      unsigned int ind = ~0U;
      for (int i = 0; i<16; ++i)
        if (!fonts[i] || (is_variable_widths[i]==is_variable_width && font_height==fonts[i][0]._height)) {
          ind = (unsigned int)i; break; // Found empty slot or cached font.
        }
      if (ind==~0U) { // No empty slots nor existing font in cache.
        fonts->assign();
        std::memmove(fonts,fonts + 1,15*sizeof(CImgList<ucharT>));
        std::memmove(is_variable_widths,is_variable_widths + 1,15*sizeof(bool));
        std::memset(fonts + (ind=15),0,sizeof(CImgList<ucharT>)); // Free a slot in cache for new font.
      }
      CImgList<ucharT> &font = fonts[ind];

      // Render requested font.
      if (!font) {
        const unsigned int padding_x = font_height<33U?1U:font_height<53U?2U:font_height<103U?3U:4U;
        is_variable_widths[ind] = is_variable_width;
        font = base_font.get_split('x',256);
        if (font_height!=font[0]._height)
          cimglist_for(font,l)
            font[l].resize(std::max(1U,font[l]._width*font_height/font[l]._height),font_height,-100,-100,
                           font[0]._height>font_height?2:5);
        if (is_variable_width) font.crop_font();
        cimglist_for(font,l) font[l].resize(font[l]._width + padding_x,-100,1,1,0,0,0.5);
        font.insert(256,0);
        cimglist_for_in(font,0,255,l) font[l].assign(font[l + 256]._width,font[l + 256]._height,1,3,1);