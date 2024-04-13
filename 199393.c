    template<typename t>
    void _eik_priority_queue_insert(CImg<charT>& state, unsigned int& siz, const t value,
                                    const unsigned int x, const unsigned int y, const unsigned int z) {
      if (state(x,y,z)>0) return;
      state(x,y,z) = 0;
      if (++siz>=_width) { if (!is_empty()) resize(_width*2,4,1,1,0); else assign(64,4); }
      (*this)(siz - 1,0) = (T)value; (*this)(siz - 1,1) = (T)x; (*this)(siz - 1,2) = (T)y; (*this)(siz - 1,3) = (T)z;
      for (unsigned int pos = siz - 1, par = 0; pos && value>(*this)(par=(pos + 1)/2 - 1,0); pos = par) {
        cimg::swap((*this)(pos,0),(*this)(par,0)); cimg::swap((*this)(pos,1),(*this)(par,1));
        cimg::swap((*this)(pos,2),(*this)(par,2)); cimg::swap((*this)(pos,3),(*this)(par,3));
      }