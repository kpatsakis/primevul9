    template<typename ti, typename tm>
    CImg<T>& draw_image(const int x0, const int y0, const int z0, const int c0,
                        const CImg<ti>& sprite, const CImg<tm>& mask, const float opacity=1,
                        const float mask_max_value=1) {
      if (is_empty() || !sprite || !mask) return *this;
      if (is_overlapped(sprite)) return draw_image(x0,y0,z0,c0,+sprite,mask,opacity,mask_max_value);
      if (is_overlapped(mask)) return draw_image(x0,y0,z0,c0,sprite,+mask,opacity,mask_max_value);
      if (mask._width!=sprite._width || mask._height!=sprite._height || mask._depth!=sprite._depth)
        throw CImgArgumentException(_cimg_instance
                                    "draw_image(): Sprite (%u,%u,%u,%u,%p) and mask (%u,%u,%u,%u,%p) have "
                                    "incompatible dimensions.",
                                    cimg_instance,
                                    sprite._width,sprite._height,sprite._depth,sprite._spectrum,sprite._data,
                                    mask._width,mask._height,mask._depth,mask._spectrum,mask._data);

      const bool bx = (x0<0), by = (y0<0), bz = (z0<0), bc = (c0<0);
      const int
        lX = sprite.width() - (x0 + sprite.width()>width()?x0 + sprite.width() - width():0) + (bx?x0:0),
        lY = sprite.height() - (y0 + sprite.height()>height()?y0 + sprite.height() - height():0) + (by?y0:0),
        lZ = sprite.depth() - (z0 + sprite.depth()>depth()?z0 + sprite.depth() - depth():0) + (bz?z0:0),
        lC = sprite.spectrum() - (c0 + sprite.spectrum()>spectrum()?c0 + sprite.spectrum() - spectrum():0) + (bc?c0:0);
      const ulongT
        coff = (bx?-x0:0) +
        (by?-y0*(ulongT)mask.width():0) +
        (bz?-z0*(ulongT)mask.width()*mask.height():0) +
        (bc?-c0*(ulongT)mask.width()*mask.height()*mask.depth():0),
        ssize = (ulongT)mask.width()*mask.height()*mask.depth()*mask.spectrum();
      const ti *ptrs = sprite._data + coff;
      const tm *ptrm = mask._data + coff;
      const ulongT
        offX = (ulongT)_width - lX,
        soffX = (ulongT)sprite._width - lX,
        offY = (ulongT)_width*(_height - lY),
        soffY = (ulongT)sprite._width*(sprite._height - lY),
        offZ = (ulongT)_width*_height*(_depth - lZ),
        soffZ = (ulongT)sprite._width*sprite._height*(sprite._depth - lZ);
      if (lX>0 && lY>0 && lZ>0 && lC>0) {
	T *ptrd = data(x0<0?0:x0,y0<0?0:y0,z0<0?0:z0,c0<0?0:c0);
        for (int c = 0; c<lC; ++c) {
          ptrm = mask._data + (ptrm - mask._data)%ssize;
          for (int z = 0; z<lZ; ++z) {
            for (int y = 0; y<lY; ++y) {
              for (int x = 0; x<lX; ++x) {
                const float mopacity = (float)(*(ptrm++)*opacity),
                  nopacity = cimg::abs(mopacity), copacity = mask_max_value - std::max(mopacity,0.0f);
                *ptrd = (T)((nopacity*(*(ptrs++)) + *ptrd*copacity)/mask_max_value);
                ++ptrd;
              }
              ptrd+=offX; ptrs+=soffX; ptrm+=soffX;
            }
            ptrd+=offY; ptrs+=soffY; ptrm+=soffY;
          }
          ptrd+=offZ; ptrs+=soffZ; ptrm+=soffZ;
        }
      }
      return *this;