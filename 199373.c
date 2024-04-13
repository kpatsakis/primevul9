      // Return type of a memory element as a string.
      CImg<charT> s_type(const unsigned int arg) const {
        CImg<charT> res;
        if (_cimg_mp_is_vector(arg)) { // Vector
          CImg<charT>::string("vectorXXXXXXXXXXXXXXXX").move_to(res);
          std::sprintf(res._data + 6,"%u",_cimg_mp_size(arg));
        } else CImg<charT>::string("scalar").move_to(res);
        return res;