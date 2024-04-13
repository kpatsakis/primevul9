    template<typename to>
    float* _object3dtoCImg3d(const CImgList<to>& opacities, float *ptrd) const {
      cimglist_for(opacities,o) {
        const CImg<to>& opacity = opacities[o];
        const to *ptro = opacity._data;
        if (opacity.size()==1) *(ptrd++) = (float)*ptro;
        else {
          *(ptrd++) = -128.0f;
          int shared_ind = -1;
          if (opacity.is_shared()) for (int i = 0; i<o; ++i) if (ptro==opacities[i]._data) { shared_ind = i; break; }
          if (shared_ind<0) {
            *(ptrd++) = (float)opacity._width;
            *(ptrd++) = (float)opacity._height;
            *(ptrd++) = (float)opacity._spectrum;
            cimg_foroff(opacity,l) *(ptrd++) = (float)*(ptro++);
          } else {
            *(ptrd++) = (float)shared_ind;
            *(ptrd++) = 0;
            *(ptrd++) = 0;
          }
        }
      }
      return ptrd;