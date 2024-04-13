    //! Return stream line of a 3d vector field \overloading.
    static CImg<floatT> streamline(const char *const expression,
                                   const float x, const float y, const float z,
                                   const float L=256, const float dl=0.1f,
                                   const unsigned int interpolation_type=2, const bool is_backward_tracking=true,
                                   const bool is_oriented_only=false,
                                   const float x0=0, const float y0=0, const float z0=0,
                                   const float x1=0, const float y1=0, const float z1=0) {
      _functor4d_streamline_expr func(expression);
      return streamline(func,x,y,z,L,dl,interpolation_type,is_backward_tracking,is_oriented_only,x0,y0,z0,x1,y1,z1);