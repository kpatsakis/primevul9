    template<typename T>
    CImgDisplay& display(const CImgList<T>& list, const char axis='x', const float align=0) {
      if (list._width==1) {
        const CImg<T>& img = list[0];
        if (img._depth==1 && (img._spectrum==1 || img._spectrum>=3) && _normalization!=1) return display(img);
      }
      CImgList<typename CImg<T>::ucharT> visu(list._width);
      unsigned int dims = 0;
      cimglist_for(list,l) {
        const CImg<T>& img = list._data[l];
        img.__get_select(*this,_normalization,(img._width - 1)/2,(img._height - 1)/2,
                         (img._depth - 1)/2).move_to(visu[l]);
        dims = std::max(dims,visu[l]._spectrum);
      }
      cimglist_for(list,l) if (visu[l]._spectrum<dims) visu[l].resize(-100,-100,-100,dims,1);
      visu.get_append(axis,align).display(*this);
      return *this;