    //! Compute a n-d Fast Fourier Transform.
    /**
      \param invert Tells if the direct (\c false) or inverse transform (\c true) is computed.
    **/
    CImgList<T>& FFT(const bool invert=false) {
      if (is_empty()) return *this;
      if (_width==1) insert(1);
      if (_width>2)
        cimg::warn(_cimglist_instance
                   "FFT(): Instance has more than 2 images",
                   cimglist_instance);