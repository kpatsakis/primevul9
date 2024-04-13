    //! Construct image with specified size and initialize pixel values from a memory buffer \specialization.
    CImg<T>& assign(const T *const values, const unsigned int size_x, const unsigned int size_y=1,
                    const unsigned int size_z=1, const unsigned int size_c=1) {
      const size_t siz = (size_t)size_x*size_y*size_z*size_c;
      if (!values || !siz) return assign();
      const size_t curr_siz = (size_t)size();
      if (values==_data && siz==curr_siz) return assign(size_x,size_y,size_z,size_c);
      if (_is_shared || values + siz<_data || values>=_data + size()) {
        assign(size_x,size_y,size_z,size_c);
        if (_is_shared) std::memmove(_data,values,siz*sizeof(T));
        else std::memcpy(_data,values,siz*sizeof(T));
      } else {
        T *new_data = 0;
        try { new_data = new T[siz]; } catch (...) {
          _width = _height = _depth = _spectrum = 0; _data = 0;
          throw CImgInstanceException(_cimg_instance
                                      "assign(): Failed to allocate memory (%s) for image (%u,%u,%u,%u).",
                                      cimg_instance,
                                      cimg::strbuffersize(sizeof(T)*size_x*size_y*size_z*size_c),
                                      size_x,size_y,size_z,size_c);
        }
        std::memcpy(new_data,values,siz*sizeof(T));
        delete[] _data; _data = new_data; _width = size_x; _height = size_y; _depth = size_z; _spectrum = size_c;
      }
      return *this;