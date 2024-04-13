    template<typename tp, typename tc, typename tt, typename tx>
    const CImg<T>& texturize_object3d(CImgList<tp>& primitives, CImgList<tc>& colors,
                                      const CImg<tt>& texture, const CImg<tx>& coords=CImg<tx>::const_empty()) const {
      if (is_empty()) return *this;
      if (_height!=3)
        throw CImgInstanceException(_cimg_instance
                                    "texturize_object3d(): image instance is not a set of 3d points.",
                                    cimg_instance);
      if (coords && (coords._width!=_width || coords._height!=2))
        throw CImgArgumentException(_cimg_instance
                                    "texturize_object3d(): Invalid specified texture coordinates (%u,%u,%u,%u,%p).",
                                    cimg_instance,
                                    coords._width,coords._height,coords._depth,coords._spectrum,coords._data);
      CImg<intT> _coords;
      if (!coords) { // If no texture coordinates specified, do a default XY-projection.
        _coords.assign(_width,2);
        float
          xmin, xmax = (float)get_shared_row(0).max_min(xmin),
          ymin, ymax = (float)get_shared_row(1).max_min(ymin),
          dx = xmax>xmin?xmax-xmin:1,
          dy = ymax>ymin?ymax-ymin:1;
        cimg_forX(*this,p) {
          _coords(p,0) = (int)(((*this)(p,0) - xmin)*texture._width/dx);
          _coords(p,1) = (int)(((*this)(p,1) - ymin)*texture._height/dy);
        }
      } else _coords = coords;

      int texture_ind = -1;
      cimglist_for(primitives,l) {
        CImg<tp> &p = primitives[l];
        const unsigned int siz = p.size();
        switch (siz) {
        case 1 : { // Point.
          const unsigned int i0 = (unsigned int)p[0];
          const int x0 = _coords(i0,0), y0 = _coords(i0,1);
          texture.get_vector_at(x0<=0?0:x0>=texture.width()?texture.width() - 1:x0,
                                y0<=0?0:y0>=texture.height()?texture.height() - 1:y0).move_to(colors[l]);
        } break;
        case 2 : case 6 : { // Line.
          const unsigned int i0 = (unsigned int)p[0], i1 = (unsigned int)p[1];
          const int
            x0 = _coords(i0,0), y0 = _coords(i0,1),
            x1 = _coords(i1,0), y1 = _coords(i1,1);
          if (texture_ind<0) colors[texture_ind=l].assign(texture,false);
          else colors[l].assign(colors[texture_ind],true);
          CImg<tp>::vector(i0,i1,x0,y0,x1,y1).move_to(p);
        } break;
        case 3 : case 9 : { // Triangle.
          const unsigned int i0 = (unsigned int)p[0], i1 = (unsigned int)p[1], i2 = (unsigned int)p[2];
          const int
            x0 = _coords(i0,0), y0 = _coords(i0,1),
            x1 = _coords(i1,0), y1 = _coords(i1,1),
            x2 = _coords(i2,0), y2 = _coords(i2,1);
          if (texture_ind<0) colors[texture_ind=l].assign(texture,false);
          else colors[l].assign(colors[texture_ind],true);
          CImg<tp>::vector(i0,i1,i2,x0,y0,x1,y1,x2,y2).move_to(p);
        } break;
        case 4 : case 12 : { // Quadrangle.
          const unsigned int
            i0 = (unsigned int)p[0], i1 = (unsigned int)p[1], i2 = (unsigned int)p[2], i3 = (unsigned int)p[3];
          const int
            x0 = _coords(i0,0), y0 = _coords(i0,1),
            x1 = _coords(i1,0), y1 = _coords(i1,1),
            x2 = _coords(i2,0), y2 = _coords(i2,1),
            x3 = _coords(i3,0), y3 = _coords(i3,1);
          if (texture_ind<0) colors[texture_ind=l].assign(texture,false);
          else colors[l].assign(colors[texture_ind],true);
          CImg<tp>::vector(i0,i1,i2,i3,x0,y0,x1,y1,x2,y2,x3,y3).move_to(p);
        } break;
        }
      }
      return *this;