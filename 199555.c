    template<typename t, typename T>
    static void _render_resize(const T *ptrs, const unsigned int ws, const unsigned int hs,
                               t *ptrd, const unsigned int wd, const unsigned int hd) {
      unsigned int *const offx = new unsigned int[wd], *const offy = new unsigned int[hd + 1], *poffx, *poffy;
      float s, curr, old;
      s = (float)ws/wd;
      poffx = offx; curr = 0; for (unsigned int x = 0; x<wd; ++x) {
        old = curr; curr+=s; *(poffx++) = (unsigned int)curr - (unsigned int)old;
      }
      s = (float)hs/hd;
      poffy = offy; curr = 0; for (unsigned int y = 0; y<hd; ++y) {
        old = curr; curr+=s; *(poffy++) = ws*((unsigned int)curr - (unsigned int)old);
      }
      *poffy = 0;
      poffy = offy;
      for (unsigned int y = 0; y<hd; ) {
        const T *ptr = ptrs;
        poffx = offx;
        for (unsigned int x = 0; x<wd; ++x) { *(ptrd++) = *ptr; ptr+=*(poffx++); }
        ++y;
        unsigned int dy = *(poffy++);
        for ( ; !dy && y<hd; std::memcpy(ptrd,ptrd - wd,sizeof(t)*wd), ++y, ptrd+=wd, dy = *(poffy++)) {}
        ptrs+=dy;
      }
      delete[] offx; delete[] offy;