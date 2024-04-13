    template<typename T>
    CImgDisplay& render(const CImg<T>& img) {
      if (!img)
        throw CImgArgumentException(_cimgdisplay_instance
                                    "render(): Empty specified image.",
                                    cimgdisplay_instance);

      if (is_empty()) return *this;
      if (img._depth!=1) return render(img.get_projections2d((img._width - 1)/2,(img._height - 1)/2,
                                                             (img._depth - 1)/2));

      const T
        *data1 = img._data,
        *data2 = (img._spectrum>=2)?img.data(0,0,0,1):data1,
        *data3 = (img._spectrum>=3)?img.data(0,0,0,2):data1;

      WaitForSingleObject(_mutex,INFINITE);
      unsigned int
        *const ndata = (img._width==_width && img._height==_height)?_data:
        new unsigned int[(size_t)img._width*img._height],
        *ptrd = ndata;

      if (!_normalization || (_normalization==3 && cimg::type<T>::string()==cimg::type<unsigned char>::string())) {
        _min = _max = 0;
        switch (img._spectrum) {
        case 1 : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char val = (unsigned char)*(data1++);
            *(ptrd++) = (unsigned int)((val<<16) | (val<<8) | val);
          }
        } break;
        case 2 : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char
              R = (unsigned char)*(data1++),
              G = (unsigned char)*(data2++);
            *(ptrd++) = (unsigned int)((R<<16) | (G<<8));
          }
        } break;
        default : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char
              R = (unsigned char)*(data1++),
              G = (unsigned char)*(data2++),
              B = (unsigned char)*(data3++);
            *(ptrd++) = (unsigned int)((R<<16) | (G<<8) | B);
          }
        }
        }
      } else {
        if (_normalization==3) {
          if (cimg::type<T>::is_float()) _min = (float)img.min_max(_max);
          else { _min = (float)cimg::type<T>::min(); _max = (float)cimg::type<T>::max(); }
        } else if ((_min>_max) || _normalization==1) _min = (float)img.min_max(_max);
        const float delta = _max - _min, mm = 255/(delta?delta:1.0f);
        switch (img._spectrum) {
        case 1 : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char val = (unsigned char)((*(data1++) - _min)*mm);
            *(ptrd++) = (unsigned int)((val<<16) | (val<<8) | val);
          }
        } break;
        case 2 : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char
              R = (unsigned char)((*(data1++) - _min)*mm),
              G = (unsigned char)((*(data2++) - _min)*mm);
            *(ptrd++) = (unsigned int)((R<<16) | (G<<8));
          }
        } break;
        default : {
          for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
            const unsigned char
              R = (unsigned char)((*(data1++) - _min)*mm),
              G = (unsigned char)((*(data2++) - _min)*mm),
              B = (unsigned char)((*(data3++) - _min)*mm);
            *(ptrd++) = (unsigned int)((R<<16) | (G<<8) | B);
          }
        }
        }
      }
      if (ndata!=_data) { _render_resize(ndata,img._width,img._height,_data,_width,_height); delete[] ndata; }
      ReleaseMutex(_mutex);
      return *this;