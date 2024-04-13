    template<typename t>
    CImgList<T> get_split(const CImg<t>& values, const char axis=0, const bool keep_values=true) const {
      CImgList<T> res;
      if (is_empty()) return res;
      const ulongT vsiz = values.size();
      const char _axis = cimg::lowercase(axis);
      if (!vsiz) return CImgList<T>(*this);
      if (vsiz==1) { // Split according to a single value.
        const T value = (T)*values;
        switch (_axis) {
        case 'x' : {
          unsigned int i0 = 0, i = 0;
          do {
            while (i<_width && (*this)(i)==value) ++i;
            if (i>i0) { if (keep_values) get_columns(i0,i - 1).move_to(res); i0 = i; }
            while (i<_width && (*this)(i)!=value) ++i;
            if (i>i0) { get_columns(i0,i - 1).move_to(res); i0 = i; }
          } while (i<_width);
        } break;
        case 'y' : {
          unsigned int i0 = 0, i = 0;
          do {
            while (i<_height && (*this)(0,i)==value) ++i;
            if (i>i0) { if (keep_values) get_rows(i0,i - 1).move_to(res); i0 = i; }
            while (i<_height && (*this)(0,i)!=value) ++i;
            if (i>i0) { get_rows(i0,i - 1).move_to(res); i0 = i; }
          } while (i<_height);
        } break;
        case 'z' : {
          unsigned int i0 = 0, i = 0;
          do {
            while (i<_depth && (*this)(0,0,i)==value) ++i;
            if (i>i0) { if (keep_values) get_slices(i0,i - 1).move_to(res); i0 = i; }
            while (i<_depth && (*this)(0,0,i)!=value) ++i;
            if (i>i0) { get_slices(i0,i - 1).move_to(res); i0 = i; }
          } while (i<_depth);
        } break;
        case 'c' : {
          unsigned int i0 = 0, i = 0;
          do {
            while (i<_spectrum && (*this)(0,0,0,i)==value) ++i;
            if (i>i0) { if (keep_values) get_channels(i0,i - 1).move_to(res); i0 = i; }
            while (i<_spectrum && (*this)(0,0,0,i)!=value) ++i;
            if (i>i0) { get_channels(i0,i - 1).move_to(res); i0 = i; }
          } while (i<_spectrum);
        } break;
        default : {
          const ulongT siz = size();
          ulongT i0 = 0, i = 0;
          do {
            while (i<siz && (*this)[i]==value) ++i;
            if (i>i0) { if (keep_values) CImg<T>(_data + i0,1,(unsigned int)(i - i0)).move_to(res); i0 = i; }
            while (i<siz && (*this)[i]!=value) ++i;
            if (i>i0) { CImg<T>(_data + i0,1,(unsigned int)(i - i0)).move_to(res); i0 = i; }
          } while (i<siz);
        }
        }
      } else { // Split according to multiple values.
        ulongT j = 0;
        switch (_axis) {
        case 'x' : {
          unsigned int i0 = 0, i1 = 0, i = 0;
          do {
            if ((*this)(i)==*values) {
              i1 = i; j = 0;
              while (i<_width && (*this)(i)==values[j]) { ++i; if (++j>=vsiz) j = 0; }
              i-=j;
              if (i>i1) {
                if (i1>i0) get_columns(i0,i1 - 1).move_to(res);
                if (keep_values) get_columns(i1,i - 1).move_to(res);
                i0 = i;
              } else ++i;
            } else ++i;
          } while (i<_width);
          if (i0<_width) get_columns(i0,width() - 1).move_to(res);
        } break;
        case 'y' : {
          unsigned int i0 = 0, i1 = 0, i = 0;
          do {
            if ((*this)(0,i)==*values) {
              i1 = i; j = 0;
              while (i<_height && (*this)(0,i)==values[j]) { ++i; if (++j>=vsiz) j = 0; }
              i-=j;
              if (i>i1) {
                if (i1>i0) get_rows(i0,i1 - 1).move_to(res);
                if (keep_values) get_rows(i1,i - 1).move_to(res);
                i0 = i;
              } else ++i;
            } else ++i;
          } while (i<_height);
          if (i0<_height) get_rows(i0,height() - 1).move_to(res);
        } break;
        case 'z' : {
          unsigned int i0 = 0, i1 = 0, i = 0;
          do {
            if ((*this)(0,0,i)==*values) {
              i1 = i; j = 0;
              while (i<_depth && (*this)(0,0,i)==values[j]) { ++i; if (++j>=vsiz) j = 0; }
              i-=j;
              if (i>i1) {
                if (i1>i0) get_slices(i0,i1 - 1).move_to(res);
                if (keep_values) get_slices(i1,i - 1).move_to(res);
                i0 = i;
              } else ++i;
            } else ++i;
          } while (i<_depth);
          if (i0<_depth) get_slices(i0,depth() - 1).move_to(res);
        } break;
        case 'c' : {
          unsigned int i0 = 0, i1 = 0, i = 0;
          do {
            if ((*this)(0,0,0,i)==*values) {
              i1 = i; j = 0;
              while (i<_spectrum && (*this)(0,0,0,i)==values[j]) { ++i; if (++j>=vsiz) j = 0; }
              i-=j;
              if (i>i1) {
                if (i1>i0) get_channels(i0,i1 - 1).move_to(res);
                if (keep_values) get_channels(i1,i - 1).move_to(res);
                i0 = i;
              } else ++i;
            } else ++i;
          } while (i<_spectrum);
          if (i0<_spectrum) get_channels(i0,spectrum() - 1).move_to(res);
        } break;
        default : {
          ulongT i0 = 0, i1 = 0, i = 0;
          const ulongT siz = size();
          do {
            if ((*this)[i]==*values) {
              i1 = i; j = 0;
              while (i<siz && (*this)[i]==values[j]) { ++i; if (++j>=vsiz) j = 0; }
              i-=j;
              if (i>i1) {
                if (i1>i0) CImg<T>(_data + i0,1,(unsigned int)(i1 - i0)).move_to(res);
                if (keep_values) CImg<T>(_data + i1,1,(unsigned int)(i - i1)).move_to(res);
                i0 = i;
              } else ++i;
            } else ++i;
          } while (i<siz);
          if (i0<siz) CImg<T>(_data + i0,1,(unsigned int)(siz - i0)).move_to(res);
        } break;
        }
      }
      return res;