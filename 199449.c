
    //! Reverse primitives orientations of a 3d object.
    /**
    **/
    CImgList<T>& reverse_object3d() {
      cimglist_for(*this,l) {
        CImg<T>& p = _data[l];
        switch (p.size()) {
        case 2 : case 3: cimg::swap(p[0],p[1]); break;
        case 6 : cimg::swap(p[0],p[1],p[2],p[4],p[3],p[5]); break;
        case 9 : cimg::swap(p[0],p[1],p[3],p[5],p[4],p[6]); break;
        case 4 : cimg::swap(p[0],p[1],p[2],p[3]); break;
        case 12 : cimg::swap(p[0],p[1],p[2],p[3],p[4],p[6],p[5],p[7],p[8],p[10],p[9],p[11]); break;