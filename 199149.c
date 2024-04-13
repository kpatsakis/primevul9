    /**
      \param pos1 Starting index of the removal.
      \param pos2 Ending index of the removal.
    **/
    CImgList<T>& remove(const unsigned int pos1, const unsigned int pos2) {
      const unsigned int
        npos1 = pos1<pos2?pos1:pos2,
        tpos2 = pos1<pos2?pos2:pos1,
        npos2 = tpos2<_width?tpos2:_width - 1;
      if (npos1>=_width)
        throw CImgArgumentException(_cimglist_instance
                                    "remove(): Invalid remove request at positions %u->%u.",
                                    cimglist_instance,
                                    npos1,tpos2);
      else {
        if (tpos2>=_width)
          throw CImgArgumentException(_cimglist_instance
                                      "remove(): Invalid remove request at positions %u->%u.",
                                      cimglist_instance,
                                      npos1,tpos2);

        for (unsigned int k = npos1; k<=npos2; ++k) _data[k].assign();
        const unsigned int nb = 1 + npos2 - npos1;
        if (!(_width-=nb)) return assign();
        if (_width>(_allocated_width>>2) || _allocated_width<=16) { // Removing items without reallocation.
          if (npos1!=_width) std::memmove(_data + npos1,_data + npos2 + 1,sizeof(CImg<T>)*(_width - npos1));
          std::memset(_data + _width,0,sizeof(CImg<T>)*nb);
        } else { // Removing items with reallocation.
          _allocated_width>>=2;
          while (_allocated_width>16 && _width<(_allocated_width>>1)) _allocated_width>>=1;
          CImg<T> *const new_data = new CImg<T>[_allocated_width];
          if (npos1) std::memcpy(new_data,_data,sizeof(CImg<T>)*npos1);
          if (npos1!=_width) std::memcpy(new_data + npos1,_data + npos2 + 1,sizeof(CImg<T>)*(_width - npos1));
          if (_width!=_allocated_width) std::memset(new_data + _width,0,sizeof(CImg<T>)*(_allocated_width - _width));
          std::memset(_data,0,sizeof(CImg<T>)*(_width + nb));
          delete[] _data;
          _data = new_data;