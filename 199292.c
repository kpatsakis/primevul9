    //! Draw an image \specialization.
    CImg<T>& draw_image(const int x0, const int y0, const int z0, const int c0,
                        const CImg<T>& sprite, const float opacity=1) {
      if (is_empty() || !sprite) return *this;
      if (is_overlapped(sprite)) return draw_image(x0,y0,z0,c0,+sprite,opacity);
      if (x0==0 && y0==0 && z0==0 && c0==0 && is_sameXYZC(sprite) && opacity>=1 && !is_shared())
        return assign(sprite,false);
      const bool bx = (x0<0), by = (y0<0), bz = (z0<0), bc = (c0<0);
      const int
        lX = sprite.width() - (x0 + sprite.width()>width()?x0 + sprite.width() - width():0) + (bx?x0:0),
        lY = sprite.height() - (y0 + sprite.height()>height()?y0 + sprite.height() - height():0) + (by?y0:0),
        lZ = sprite.depth() - (z0 + sprite.depth()>depth()?z0 + sprite.depth() - depth():0) + (bz?z0:0),
        lC = sprite.spectrum() - (c0 + sprite.spectrum()>spectrum()?c0 + sprite.spectrum() - spectrum():0) + (bc?c0:0);
      const T
        *ptrs = sprite._data +
        (bx?-x0:0) +
        (by?-y0*(ulongT)sprite.width():0) +
        (bz?-z0*(ulongT)sprite.width()*sprite.height():0) +
        (bc?-c0*(ulongT)sprite.width()*sprite.height()*sprite.depth():0);
      const ulongT
        offX = (ulongT)_width - lX,
        soffX = (ulongT)sprite._width - lX,
        offY = (ulongT)_width*(_height - lY),
        soffY = (ulongT)sprite._width*(sprite._height - lY),
        offZ = (ulongT)_width*_height*(_depth - lZ),
        soffZ = (ulongT)sprite._width*sprite._height*(sprite._depth - lZ),
        slX = lX*sizeof(T);
      const float nopacity = cimg::abs(opacity), copacity = 1 - std::max(opacity,0.0f);
      if (lX>0 && lY>0 && lZ>0 && lC>0) {
        T *ptrd = data(x0<0?0:x0,y0<0?0:y0,z0<0?0:z0,c0<0?0:c0);
        for (int v = 0; v<lC; ++v) {
          for (int z = 0; z<lZ; ++z) {
            if (opacity>=1)
              for (int y = 0; y<lY; ++y) { std::memcpy(ptrd,ptrs,slX); ptrd+=_width; ptrs+=sprite._width; }
            else for (int y = 0; y<lY; ++y) {
                for (int x = 0; x<lX; ++x) { *ptrd = (T)(nopacity*(*(ptrs++)) + *ptrd*copacity); ++ptrd; }
                ptrd+=offX; ptrs+=soffX;
              }
            ptrd+=offY; ptrs+=soffY;
          }
          ptrd+=offZ; ptrs+=soffZ;
        }
      }
      return *this;