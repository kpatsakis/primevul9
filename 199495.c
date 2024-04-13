      return *this;
    }

    // [internal] Return string to describe display title.
    CImg<charT> __display() const {
      CImg<charT> res, str;
      cimglist_for(*this,l) {
        CImg<charT>::string(_data[l]).move_to(str);
        if (l!=width() - 1) {
          str.resize(str._width + 1,1,1,1,0);
          str[str._width - 2] = ',';
          str[str._width - 1] = ' ';
        }
        res.append(str,'x');
      }
      if (!res) return CImg<charT>(1,1,1,1,0).move_to(res);
      cimg::strellipsize(res,128,false);
      if (_width>1) {
        const unsigned int l = (unsigned int)std::strlen(res);
        if (res._width<=l + 16) res.resize(l + 16,1,1,1,0);