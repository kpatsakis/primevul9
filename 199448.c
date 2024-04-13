    //! Label connected components \newinstance.
    CImg<ulongT> get_label(const bool is_high_connectivity=false,
                           const Tfloat tolerance=0) const {
      if (is_empty()) return CImg<ulongT>();

      // Create neighborhood tables.
      int dx[13], dy[13], dz[13], nb = 0;
      dx[nb] = 1; dy[nb] = 0; dz[nb++] = 0;
      dx[nb] = 0; dy[nb] = 1; dz[nb++] = 0;
      if (is_high_connectivity) {
        dx[nb] = 1; dy[nb] = 1; dz[nb++] = 0;
        dx[nb] = 1; dy[nb] = -1; dz[nb++] = 0;
      }
      if (_depth>1) { // 3d version.
        dx[nb] = 0; dy[nb] = 0; dz[nb++]=1;
        if (is_high_connectivity) {
          dx[nb] = 1; dy[nb] = 1; dz[nb++] = -1;
          dx[nb] = 1; dy[nb] = 0; dz[nb++] = -1;
          dx[nb] = 1; dy[nb] = -1; dz[nb++] = -1;
          dx[nb] = 0; dy[nb] = 1; dz[nb++] = -1;

          dx[nb] = 0; dy[nb] = 1; dz[nb++] = 1;
          dx[nb] = 1; dy[nb] = -1; dz[nb++] = 1;
          dx[nb] = 1; dy[nb] = 0; dz[nb++] = 1;
          dx[nb] = 1; dy[nb] = 1; dz[nb++] = 1;
        }
      }
      return _label(nb,dx,dy,dz,tolerance);