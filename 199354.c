    **/
    CImg<T>& autocrop(const T *const color=0, const char *const axes="zyx") {
      if (is_empty()) return *this;
      if (!color) { // Guess color.
        const CImg<T> col1 = get_vector_at(0,0,0);
        const unsigned int w = _width, h = _height, d = _depth, s = _spectrum;
        autocrop(col1,axes);
        if (_width==w && _height==h && _depth==d && _spectrum==s) {
          const CImg<T> col2 = get_vector_at(w - 1,h - 1,d - 1);
          autocrop(col2,axes);
        }
        return *this;
      }
      for (const char *s = axes; *s; ++s) {
        const char axis = cimg::lowercase(*s);
        switch (axis) {
        case 'x' : {
	  int x0 = width(), x1 = -1;
	  cimg_forC(*this,c) {
	    const CImg<intT> coords = get_shared_channel(c)._autocrop(color[c],'x');
	    const int nx0 = coords[0], nx1 = coords[1];
	    if (nx0>=0 && nx1>=0) { x0 = std::min(x0,nx0); x1 = std::max(x1,nx1); }
	  }
          if (x0==width() && x1==-1) return assign(); else crop(x0,x1);
	} break;
        case 'y' : {
	  int y0 = height(), y1 = -1;
	  cimg_forC(*this,c) {
	    const CImg<intT> coords = get_shared_channel(c)._autocrop(color[c],'y');
	    const int ny0 = coords[0], ny1 = coords[1];
	    if (ny0>=0 && ny1>=0) { y0 = std::min(y0,ny0); y1 = std::max(y1,ny1); }
	  }
          if (y0==height() && y1==-1) return assign(); else crop(0,y0,_width - 1,y1);
	} break;
        default : {
	  int z0 = depth(), z1 = -1;
	  cimg_forC(*this,c) {
	    const CImg<intT> coords = get_shared_channel(c)._autocrop(color[c],'z');
	    const int nz0 = coords[0], nz1 = coords[1];
	    if (nz0>=0 && nz1>=0) { z0 = std::min(z0,nz0); z1 = std::max(z1,nz1); }
	  }
	  if (z0==depth() && z1==-1) return assign(); else crop(0,0,z0,_width - 1,_height - 1,z1);
	}
        }
      }
      return *this;