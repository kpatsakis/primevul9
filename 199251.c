
    //! Crop font along the X-axis \newinstance.
    /**
    **/
    CImgList<T> get_crop_font() const {
      CImgList<T> res;
      cimglist_for(*this,l) {
        const CImg<T>& letter = (*this)[l];
        int xmin = letter.width(), xmax = 0;
        cimg_forXY(letter,x,y) if (letter(x,y)) { if (x<xmin) xmin = x; if (x>xmax) xmax = x; }
        if (xmin>xmax) CImg<T>(letter._width,letter._height,1,letter._spectrum,0).move_to(res);
        else letter.get_crop(xmin,0,xmax,letter._height - 1).move_to(res);
      }