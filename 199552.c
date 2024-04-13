    // Compute SSD between two patches in different images.
    static float _patchmatch(const CImg<T>& img1, const CImg<T>& img2,
                             const unsigned int psizew, const unsigned int psizeh,
                             const int x1, const int y1,
                             const int x2, const int y2,
                             const float max_ssd) { // 2d version.
      const T *p1 = img1.data(x1,y1), *p2 = img2.data(x2,y2);
      const ulongT
        offx1 = (ulongT)img1._width - psizew,
        offx2 = (ulongT)img2._width - psizew,
        offy1 = (ulongT)img1._width*img1._height - psizeh*img1._width,
        offy2 = (ulongT)img2._width*img2._height - psizeh*img2._width;
      float ssd = 0;
      cimg_forC(img1,c) {
        for (unsigned int j = 0; j<psizeh; ++j) {
          for (unsigned int i = 0; i<psizew; ++i)
            ssd += cimg::sqr(*(p1++) - *(p2++));
          if (ssd>max_ssd) return max_ssd;
          p1+=offx1; p2+=offx2;
        }
        p1+=offy1; p2+=offy2;
      }
      return ssd;