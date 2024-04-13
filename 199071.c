      return *this;
    }

    //! Insert a copy of the image \c img into the current image list, at position \c pos \specialization.
    CImgList<T>& insert(const CImg<T>& img, const unsigned int pos=~0U, const bool is_shared=false) {
      const unsigned int npos = pos==~0U?_width:pos;
      if (npos>_width)
        throw CImgArgumentException(_cimglist_instance
                                    "insert(): Invalid insertion request of specified image (%u,%u,%u,%u,%p) "
                                    "at position %u.",
                                    cimglist_instance,
                                    img._width,img._height,img._depth,img._spectrum,img._data,npos);
      CImg<T> *const new_data = (++_width>_allocated_width)?new CImg<T>[_allocated_width?(_allocated_width<<=1):
                                                                        (_allocated_width=16)]:0;
      if (!_data) { // Insert new element into empty list.
        _data = new_data;
        if (is_shared && img) {
          _data->_width = img._width;
          _data->_height = img._height;
          _data->_depth = img._depth;
          _data->_spectrum = img._spectrum;
          _data->_is_shared = true;
          _data->_data = img._data;
        } else *_data = img;
      }
      else {
        if (new_data) { // Insert with re-allocation.
          if (npos) std::memcpy(new_data,_data,sizeof(CImg<T>)*npos);
          if (npos!=_width - 1) std::memcpy(new_data + npos + 1,_data + npos,sizeof(CImg<T>)*(_width - 1 - npos));
          if (is_shared && img) {
            new_data[npos]._width = img._width;
            new_data[npos]._height = img._height;
            new_data[npos]._depth = img._depth;
            new_data[npos]._spectrum = img._spectrum;
            new_data[npos]._is_shared = true;
            new_data[npos]._data = img._data;
          } else {
            new_data[npos]._width = new_data[npos]._height = new_data[npos]._depth = new_data[npos]._spectrum = 0;
            new_data[npos]._data = 0;
            new_data[npos] = img;
          }
          std::memset(_data,0,sizeof(CImg<T>)*(_width - 1));
          delete[] _data;
          _data = new_data;
        } else { // Insert without re-allocation.
          if (npos!=_width - 1) std::memmove(_data + npos + 1,_data + npos,sizeof(CImg<T>)*(_width - 1 - npos));
          if (is_shared && img) {
            _data[npos]._width = img._width;
            _data[npos]._height = img._height;
            _data[npos]._depth = img._depth;
            _data[npos]._spectrum = img._spectrum;
            _data[npos]._is_shared = true;
            _data[npos]._data = img._data;
          } else {
            _data[npos]._width = _data[npos]._height = _data[npos]._depth = _data[npos]._spectrum = 0;
            _data[npos]._data = 0;
            _data[npos] = img;
          }