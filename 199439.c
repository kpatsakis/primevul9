
    CImg<T>& _priority_queue_remove(unsigned int& siz) {
      (*this)(0,0) = (*this)(--siz,0);
      (*this)(0,1) = (*this)(siz,1);
      (*this)(0,2) = (*this)(siz,2);
      (*this)(0,3) = (*this)(siz,3);
      const float value = (*this)(0,0);
      for (unsigned int pos = 0, left = 0, right = 0;
           ((right=2*(pos + 1),(left=right - 1))<siz && value<(*this)(left,0)) ||
             (right<siz && value<(*this)(right,0));) {
        if (right<siz) {
          if ((*this)(left,0)>(*this)(right,0)) {
            cimg::swap((*this)(pos,0),(*this)(left,0));
            cimg::swap((*this)(pos,1),(*this)(left,1));
            cimg::swap((*this)(pos,2),(*this)(left,2));
            cimg::swap((*this)(pos,3),(*this)(left,3));
            pos = left;
          } else {
            cimg::swap((*this)(pos,0),(*this)(right,0));
            cimg::swap((*this)(pos,1),(*this)(right,1));
            cimg::swap((*this)(pos,2),(*this)(right,2));
            cimg::swap((*this)(pos,3),(*this)(right,3));
            pos = right;
          }
        } else {
          cimg::swap((*this)(pos,0),(*this)(left,0));
          cimg::swap((*this)(pos,1),(*this)(left,1));
          cimg::swap((*this)(pos,2),(*this)(left,2));
          cimg::swap((*this)(pos,3),(*this)(left,3));
          pos = left;
        }
      }
      return *this;