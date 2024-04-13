    template<typename T>
    CImgDisplay& render(const CImg<T>& img, const bool flag8=false) {
      if (!img)
        throw CImgArgumentException(_cimgdisplay_instance
                                    "render(): Empty specified image.",
                                    cimgdisplay_instance);
      if (is_empty()) return *this;
      if (img._depth!=1) return render(img.get_projections2d((img._width - 1)/2,(img._height - 1)/2,
                                                             (img._depth - 1)/2));
      if (cimg::X11_attr().nb_bits==8 && (img._width!=_width || img._height!=_height))
        return render(img.get_resize(_width,_height,1,-100,1));
      if (cimg::X11_attr().nb_bits==8 && !flag8 && img._spectrum==3) {
        static const CImg<typename CImg<T>::ucharT> default_colormap = CImg<typename CImg<T>::ucharT>::default_LUT256();
        return render(img.get_index(default_colormap,1,false));
      }

      const T
        *data1 = img._data,
        *data2 = (img._spectrum>1)?img.data(0,0,0,1):data1,
        *data3 = (img._spectrum>2)?img.data(0,0,0,2):data1;

      if (cimg::X11_attr().is_blue_first) cimg::swap(data1,data3);
      cimg_lock_display();

      if (!_normalization || (_normalization==3 && cimg::type<T>::string()==cimg::type<unsigned char>::string())) {
        _min = _max = 0;
        switch (cimg::X11_attr().nb_bits) {
        case 8 : { // 256 colormap, no normalization
          _set_colormap(_colormap,img._spectrum);
          unsigned char
            *const ndata = (img._width==_width && img._height==_height)?(unsigned char*)_data:
            new unsigned char[(size_t)img._width*img._height],
            *ptrd = (unsigned char*)ndata;
          switch (img._spectrum) {
          case 1 :
            for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
              (*ptrd++) = (unsigned char)*(data1++);
            break;
          case 2 : for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
	      const unsigned char
                R = (unsigned char)*(data1++),
                G = (unsigned char)*(data2++);
	      (*ptrd++) = (R&0xf0) | (G>>4);
	    } break;
          default : for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
	      const unsigned char
                R = (unsigned char)*(data1++),
                G = (unsigned char)*(data2++),
                B = (unsigned char)*(data3++);
	      (*ptrd++) = (R&0xe0) | ((G>>5)<<2) | (B>>6);
	    }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned char*)_data,_width,_height);
            delete[] ndata;
          }
        } break;
        case 16 : { // 16 bits colors, no normalization
          unsigned short *const ndata = (img._width==_width && img._height==_height)?(unsigned short*)_data:
            new unsigned short[(size_t)img._width*img._height];
          unsigned char *ptrd = (unsigned char*)ndata;
          const unsigned int M = 248;
          switch (img._spectrum) {
          case 1 :
            if (cimg::X11_attr().byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char val = (unsigned char)*(data1++), G = val>>2;
                ptrd[0] = (val&M) | (G>>3);
                ptrd[1] = (G<<5) | (G>>1);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char val = (unsigned char)*(data1++), G = val>>2;
                ptrd[0] = (G<<5) | (G>>1);
                ptrd[1] = (val&M) | (G>>3);
                ptrd+=2;
              }
            break;
          case 2 :
            if (cimg::X11_attr().byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)*(data2++)>>2;
                ptrd[0] = ((unsigned char)*(data1++)&M) | (G>>3);
                ptrd[1] = (G<<5);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)*(data2++)>>2;
                ptrd[0] = (G<<5);
                ptrd[1] = ((unsigned char)*(data1++)&M) | (G>>3);
                ptrd+=2;
              }
            break;
          default :
            if (cimg::X11_attr().byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)*(data2++)>>2;
                ptrd[0] = ((unsigned char)*(data1++)&M) | (G>>3);
                ptrd[1] = (G<<5) | ((unsigned char)*(data3++)>>3);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)*(data2++)>>2;
                ptrd[0] = (G<<5) | ((unsigned char)*(data3++)>>3);
                ptrd[1] = ((unsigned char)*(data1++)&M) | (G>>3);
                ptrd+=2;
              }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned short*)_data,_width,_height);
            delete[] ndata;
          }
        } break;
        default : { // 24 bits colors, no normalization
          unsigned int *const ndata = (img._width==_width && img._height==_height)?(unsigned int*)_data:
            new unsigned int[(size_t)img._width*img._height];
          if (sizeof(int)==4) { // 32 bits int uses optimized version
            unsigned int *ptrd = ndata;
            switch (img._spectrum) {
            case 1 :
              if (cimg::X11_attr().byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  const unsigned char val = (unsigned char)*(data1++);
                  *(ptrd++) = (val<<16) | (val<<8) | val;
                }
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                 const unsigned char val = (unsigned char)*(data1++);
                  *(ptrd++) = (val<<16) | (val<<8) | val;
                }
              break;
            case 2 :
              if (cimg::X11_attr().byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) = ((unsigned char)*(data1++)<<16) | ((unsigned char)*(data2++)<<8);
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) = ((unsigned char)*(data2++)<<16) | ((unsigned char)*(data1++)<<8);
              break;
            default :
              if (cimg::X11_attr().byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) = ((unsigned char)*(data1++)<<16) | ((unsigned char)*(data2++)<<8) |
                    (unsigned char)*(data3++);
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) = ((unsigned char)*(data3++)<<24) | ((unsigned char)*(data2++)<<16) |
                    ((unsigned char)*(data1++)<<8);
            }
          } else {
            unsigned char *ptrd = (unsigned char*)ndata;
            switch (img._spectrum) {
            case 1 :
              if (cimg::X11_attr().byte_order)
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = 0;
                  ptrd[1] = (unsigned char)*(data1++);
                  ptrd[2] = 0;
                  ptrd[3] = 0;
                  ptrd+=4;
                } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = 0;
                  ptrd[1] = 0;
                  ptrd[2] = (unsigned char)*(data1++);
                  ptrd[3] = 0;
                  ptrd+=4;
                }
              break;
            case 2 :
              if (cimg::X11_attr().byte_order) cimg::swap(data1,data2);
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                ptrd[0] = 0;
                ptrd[1] = (unsigned char)*(data2++);
                ptrd[2] = (unsigned char)*(data1++);
                ptrd[3] = 0;
                ptrd+=4;
              }
              break;
            default :
              if (cimg::X11_attr().byte_order)
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = 0;
                  ptrd[1] = (unsigned char)*(data1++);
                  ptrd[2] = (unsigned char)*(data2++);
                  ptrd[3] = (unsigned char)*(data3++);
                  ptrd+=4;
                } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = (unsigned char)*(data3++);
                  ptrd[1] = (unsigned char)*(data2++);
                  ptrd[2] = (unsigned char)*(data1++);
                  ptrd[3] = 0;
                  ptrd+=4;
                }
            }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned int*)_data,_width,_height);
            delete[] ndata;
          }
        }
        }
      } else {
        if (_normalization==3) {
          if (cimg::type<T>::is_float()) _min = (float)img.min_max(_max);
          else { _min = (float)cimg::type<T>::min(); _max = (float)cimg::type<T>::max(); }
        } else if ((_min>_max) || _normalization==1) _min = (float)img.min_max(_max);
        const float delta = _max - _min, mm = 255/(delta?delta:1.0f);
        switch (cimg::X11_attr().nb_bits) {
        case 8 : { // 256 colormap, with normalization
          _set_colormap(_colormap,img._spectrum);
          unsigned char *const ndata = (img._width==_width && img._height==_height)?(unsigned char*)_data:
            new unsigned char[(size_t)img._width*img._height];
          unsigned char *ptrd = (unsigned char*)ndata;
          switch (img._spectrum) {
          case 1 : for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
              const unsigned char R = (unsigned char)((*(data1++) - _min)*mm);
              *(ptrd++) = R;
            } break;
          case 2 : for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
              const unsigned char
                R = (unsigned char)((*(data1++) - _min)*mm),
                G = (unsigned char)((*(data2++) - _min)*mm);
            (*ptrd++) = (R&0xf0) | (G>>4);
          } break;
          default :
            for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
              const unsigned char
                R = (unsigned char)((*(data1++) - _min)*mm),
                G = (unsigned char)((*(data2++) - _min)*mm),
                B = (unsigned char)((*(data3++) - _min)*mm);
              *(ptrd++) = (R&0xe0) | ((G>>5)<<2) | (B>>6);
            }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned char*)_data,_width,_height);
            delete[] ndata;
          }
        } break;
        case 16 : { // 16 bits colors, with normalization
          unsigned short *const ndata = (img._width==_width && img._height==_height)?(unsigned short*)_data:
            new unsigned short[(size_t)img._width*img._height];
          unsigned char *ptrd = (unsigned char*)ndata;
          const unsigned int M = 248;
          switch (img._spectrum) {
          case 1 :
            if (cimg::X11_attr().byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char val = (unsigned char)((*(data1++) - _min)*mm), G = val>>2;
                ptrd[0] = (val&M) | (G>>3);
                ptrd[1] = (G<<5) | (val>>3);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char val = (unsigned char)((*(data1++) - _min)*mm), G = val>>2;
                ptrd[0] = (G<<5) | (val>>3);
                ptrd[1] = (val&M) | (G>>3);
                ptrd+=2;
              }
            break;
          case 2 :
            if (cimg::X11_attr().byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)((*(data2++) - _min)*mm)>>2;
                ptrd[0] = ((unsigned char)((*(data1++) - _min)*mm)&M) | (G>>3);
                ptrd[1] = (G<<5);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)((*(data2++) - _min)*mm)>>2;
                ptrd[0] = (G<<5);
                ptrd[1] = ((unsigned char)((*(data1++) - _min)*mm)&M) | (G>>3);
                ptrd+=2;
              }
            break;
          default :
            if (cimg::X11_attr().byte_order)
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)((*(data2++) - _min)*mm)>>2;
                ptrd[0] = ((unsigned char)((*(data1++) - _min)*mm)&M) | (G>>3);
                ptrd[1] = (G<<5) | ((unsigned char)((*(data3++) - _min)*mm)>>3);
                ptrd+=2;
              } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                const unsigned char G = (unsigned char)((*(data2++) - _min)*mm)>>2;
                ptrd[0] = (G<<5) | ((unsigned char)((*(data3++) - _min)*mm)>>3);
                ptrd[1] = ((unsigned char)((*(data1++) - _min)*mm)&M) | (G>>3);
                ptrd+=2;
              }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned short*)_data,_width,_height);
            delete[] ndata;
          }
        } break;
        default : { // 24 bits colors, with normalization
          unsigned int *const ndata = (img._width==_width && img._height==_height)?(unsigned int*)_data:
            new unsigned int[(size_t)img._width*img._height];
          if (sizeof(int)==4) { // 32 bits int uses optimized version
            unsigned int *ptrd = ndata;
            switch (img._spectrum) {
            case 1 :
              if (cimg::X11_attr().byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  const unsigned char val = (unsigned char)((*(data1++) - _min)*mm);
                  *(ptrd++) = (val<<16) | (val<<8) | val;
                }
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  const unsigned char val = (unsigned char)((*(data1++) - _min)*mm);
                  *(ptrd++) = (val<<24) | (val<<16) | (val<<8);
                }
              break;
            case 2 :
              if (cimg::X11_attr().byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) =
                    ((unsigned char)((*(data1++) - _min)*mm)<<16) |
                    ((unsigned char)((*(data2++) - _min)*mm)<<8);
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) =
                    ((unsigned char)((*(data2++) - _min)*mm)<<16) |
                    ((unsigned char)((*(data1++) - _min)*mm)<<8);
              break;
            default :
              if (cimg::X11_attr().byte_order==cimg::endianness())
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) =
                    ((unsigned char)((*(data1++) - _min)*mm)<<16) |
                    ((unsigned char)((*(data2++) - _min)*mm)<<8) |
                    (unsigned char)((*(data3++) - _min)*mm);
              else
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy)
                  *(ptrd++) =
                    ((unsigned char)((*(data3++) - _min)*mm)<<24) |
                    ((unsigned char)((*(data2++) - _min)*mm)<<16) |
                    ((unsigned char)((*(data1++) - _min)*mm)<<8);
            }
          } else {
            unsigned char *ptrd = (unsigned char*)ndata;
            switch (img._spectrum) {
            case 1 :
              if (cimg::X11_attr().byte_order)
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  const unsigned char val = (unsigned char)((*(data1++) - _min)*mm);
                  ptrd[0] = 0;
                  ptrd[1] = val;
                  ptrd[2] = val;
                  ptrd[3] = val;
                  ptrd+=4;
                } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  const unsigned char val = (unsigned char)((*(data1++) - _min)*mm);
                  ptrd[0] = val;
                  ptrd[1] = val;
                  ptrd[2] = val;
                  ptrd[3] = 0;
                  ptrd+=4;
                }
              break;
            case 2 :
              if (cimg::X11_attr().byte_order) cimg::swap(data1,data2);
              for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                ptrd[0] = 0;
                ptrd[1] = (unsigned char)((*(data2++) - _min)*mm);
                ptrd[2] = (unsigned char)((*(data1++) - _min)*mm);
                ptrd[3] = 0;
                ptrd+=4;
              }
              break;
            default :
              if (cimg::X11_attr().byte_order)
                for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = 0;
                  ptrd[1] = (unsigned char)((*(data1++) - _min)*mm);
                  ptrd[2] = (unsigned char)((*(data2++) - _min)*mm);
                  ptrd[3] = (unsigned char)((*(data3++) - _min)*mm);
                  ptrd+=4;
                } else for (cimg_ulong xy = (cimg_ulong)img._width*img._height; xy>0; --xy) {
                  ptrd[0] = (unsigned char)((*(data3++) - _min)*mm);
                  ptrd[1] = (unsigned char)((*(data2++) - _min)*mm);
                  ptrd[2] = (unsigned char)((*(data1++) - _min)*mm);
                  ptrd[3] = 0;
                  ptrd+=4;
                }
            }
          }
          if (ndata!=_data) {
            _render_resize(ndata,img._width,img._height,(unsigned int*)_data,_width,_height);
            delete[] ndata;
          }
	}
        }
      }
      cimg_unlock_display();
      return *this;