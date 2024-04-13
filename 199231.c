    /**
       \param axis Appending axis. Can be <tt>{ 'x' | 'y' | 'z' | 'c' }</tt>.
       \param align Appending alignment.
    **/
    CImg<T> get_append(const char axis, const float align=0) const {
      if (is_empty()) return CImg<T>();
      if (_width==1) return +((*this)[0]);
      unsigned int dx = 0, dy = 0, dz = 0, dc = 0, pos = 0;
      CImg<T> res;
      switch (cimg::lowercase(axis)) {
      case 'x' : { // Along the X-axis.
        cimglist_for(*this,l) {
          const CImg<T>& img = (*this)[l];
          if (img) {
            dx+=img._width;
            dy = std::max(dy,img._height);
            dz = std::max(dz,img._depth);
            dc = std::max(dc,img._spectrum);
          }
        }
        res.assign(dx,dy,dz,dc,(T)0);
        if (res) cimglist_for(*this,l) {
            const CImg<T>& img = (*this)[l];
            if (img) res.draw_image(pos,
                                    (int)(align*(dy - img._height)),
                                    (int)(align*(dz - img._depth)),
                                    (int)(align*(dc - img._spectrum)),
                                    img);
            pos+=img._width;
          }
      } break;
      case 'y' : { // Along the Y-axis.
        cimglist_for(*this,l) {
          const CImg<T>& img = (*this)[l];
          if (img) {
            dx = std::max(dx,img._width);
            dy+=img._height;
            dz = std::max(dz,img._depth);
            dc = std::max(dc,img._spectrum);
          }
        }
        res.assign(dx,dy,dz,dc,(T)0);
        if (res) cimglist_for(*this,l) {
            const CImg<T>& img = (*this)[l];
            if (img) res.draw_image((int)(align*(dx - img._width)),
                                    pos,
                                    (int)(align*(dz - img._depth)),
                                    (int)(align*(dc - img._spectrum)),
                                    img);
            pos+=img._height;
          }
      } break;
      case 'z' : { // Along the Z-axis.
        cimglist_for(*this,l) {
          const CImg<T>& img = (*this)[l];
          if (img) {
            dx = std::max(dx,img._width);
            dy = std::max(dy,img._height);
            dz+=img._depth;
            dc = std::max(dc,img._spectrum);
          }
        }
        res.assign(dx,dy,dz,dc,(T)0);
        if (res) cimglist_for(*this,l) {
            const CImg<T>& img = (*this)[l];
            if (img) res.draw_image((int)(align*(dx - img._width)),
                                    (int)(align*(dy - img._height)),
                                    pos,
                                    (int)(align*(dc - img._spectrum)),
                                    img);
            pos+=img._depth;
          }
      } break;
      default : { // Along the C-axis.
        cimglist_for(*this,l) {
          const CImg<T>& img = (*this)[l];
          if (img) {
            dx = std::max(dx,img._width);
            dy = std::max(dy,img._height);
            dz = std::max(dz,img._depth);
            dc+=img._spectrum;
          }
        }
        res.assign(dx,dy,dz,dc,(T)0);
        if (res) cimglist_for(*this,l) {
            const CImg<T>& img = (*this)[l];
            if (img) res.draw_image((int)(align*(dx - img._width)),
                                    (int)(align*(dy - img._height)),
                                    (int)(align*(dz - img._depth)),
                                    pos,
                                    img);
            pos+=img._spectrum;
          }