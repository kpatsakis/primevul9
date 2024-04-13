        \param pos Index of the insertion.
        \param is_shared Tells if the inserted image is a shared copy of \c img or not.
    **/
    template<typename t>
    CImgList<T>& insert(const CImg<t>& img, const unsigned int pos=~0U, const bool is_shared=false) {
      const unsigned int npos = pos==~0U?_width:pos;
      if (npos>_width)
        throw CImgArgumentException(_cimglist_instance
                                    "insert(): Invalid insertion request of specified image (%u,%u,%u,%u,%p) "
                                    "at position %u.",
                                    cimglist_instance,
                                    img._width,img._height,img._depth,img._spectrum,img._data,npos);
      if (is_shared)
        throw CImgArgumentException(_cimglist_instance
                                    "insert(): Invalid insertion request of specified shared image "
                                    "CImg<%s>(%u,%u,%u,%u,%p) at position %u (pixel types are different).",
                                    cimglist_instance,
                                    img.pixel_type(),img._width,img._height,img._depth,img._spectrum,img._data,npos);

      CImg<T> *const new_data = (++_width>_allocated_width)?new CImg<T>[_allocated_width?(_allocated_width<<=1):
                                                                        (_allocated_width=16)]:0;
      if (!_data) { // Insert new element into empty list.
        _data = new_data;
        *_data = img;
      } else {
        if (new_data) { // Insert with re-allocation.
          if (npos) std::memcpy(new_data,_data,sizeof(CImg<T>)*npos);
          if (npos!=_width - 1) std::memcpy(new_data + npos + 1,_data + npos,sizeof(CImg<T>)*(_width - 1 - npos));
          std::memset(_data,0,sizeof(CImg<T>)*(_width - 1));
          delete[] _data;
          _data = new_data;
        } else if (npos!=_width - 1) // Insert without re-allocation.
          std::memmove(_data + npos + 1,_data + npos,sizeof(CImg<T>)*(_width - 1 - npos));
        _data[npos]._width = _data[npos]._height = _data[npos]._depth = _data[npos]._spectrum = 0;
        _data[npos]._data = 0;