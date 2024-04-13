    template<typename tfunc>
    static CImg<floatT> streamline(const tfunc& func,
                                   const float x, const float y, const float z,
                                   const float L=256, const float dl=0.1f,
                                   const unsigned int interpolation_type=2, const bool is_backward_tracking=false,
                                   const bool is_oriented_only=false,
                                   const float x0=0, const float y0=0, const float z0=0,
                                   const float x1=0, const float y1=0, const float z1=0) {
      if (dl<=0)
        throw CImgArgumentException("CImg<%s>::streamline(): Invalid specified integration length %g "
                                    "(should be >0).",
                                    pixel_type(),
                                    dl);

      const bool is_bounded = (x0!=x1 || y0!=y1 || z0!=z1);
      if (L<=0 || (is_bounded && (x<x0 || x>x1 || y<y0 || y>y1 || z<z0 || z>z1))) return CImg<floatT>();
      const unsigned int size_L = (unsigned int)cimg::round(L/dl + 1);
      CImg<floatT> coordinates(size_L,3);
      const float dl2 = dl/2;
      float
        *ptr_x = coordinates.data(0,0),
        *ptr_y = coordinates.data(0,1),
        *ptr_z = coordinates.data(0,2),
        pu = (float)(dl*func(x,y,z,0)),
        pv = (float)(dl*func(x,y,z,1)),
        pw = (float)(dl*func(x,y,z,2)),
        X = x, Y = y, Z = z;

      switch (interpolation_type) {
      case 0 : { // Nearest integer interpolation.
        cimg_forX(coordinates,l) {
          *(ptr_x++) = X; *(ptr_y++) = Y; *(ptr_z++) = Z;
          const int
            xi = (int)(X>0?X + 0.5f:X - 0.5f),
            yi = (int)(Y>0?Y + 0.5f:Y - 0.5f),
            zi = (int)(Z>0?Z + 0.5f:Z - 0.5f);
          float
            u = (float)(dl*func((float)xi,(float)yi,(float)zi,0)),
            v = (float)(dl*func((float)xi,(float)yi,(float)zi,1)),
            w = (float)(dl*func((float)xi,(float)yi,(float)zi,2));
          if (is_oriented_only && u*pu + v*pv + w*pw<0) { u = -u; v = -v; w = -w; }
          if (is_backward_tracking) { X-=(pu=u); Y-=(pv=v); Z-=(pw=w); } else { X+=(pu=u); Y+=(pv=v); Z+=(pw=w); }
          if (is_bounded && (X<x0 || X>x1 || Y<y0 || Y>y1 || Z<z0 || Z>z1)) break;
        }
      } break;
      case 1 : { // First-order interpolation.
        cimg_forX(coordinates,l) {
          *(ptr_x++) = X; *(ptr_y++) = Y; *(ptr_z++) = Z;
          float
            u = (float)(dl*func(X,Y,Z,0)),
            v = (float)(dl*func(X,Y,Z,1)),
            w = (float)(dl*func(X,Y,Z,2));
          if (is_oriented_only && u*pu + v*pv + w*pw<0) { u = -u; v = -v; w = -w; }
          if (is_backward_tracking) { X-=(pu=u); Y-=(pv=v); Z-=(pw=w); } else { X+=(pu=u); Y+=(pv=v); Z+=(pw=w); }
          if (is_bounded && (X<x0 || X>x1 || Y<y0 || Y>y1 || Z<z0 || Z>z1)) break;
        }
      } break;
      case 2 : { // Second order interpolation.
        cimg_forX(coordinates,l) {
          *(ptr_x++) = X; *(ptr_y++) = Y; *(ptr_z++) = Z;
          float
            u0 = (float)(dl2*func(X,Y,Z,0)),
            v0 = (float)(dl2*func(X,Y,Z,1)),
            w0 = (float)(dl2*func(X,Y,Z,2));
          if (is_oriented_only && u0*pu + v0*pv + w0*pw<0) { u0 = -u0; v0 = -v0; w0 = -w0; }
          float
            u = (float)(dl*func(X + u0,Y + v0,Z + w0,0)),
            v = (float)(dl*func(X + u0,Y + v0,Z + w0,1)),
            w = (float)(dl*func(X + u0,Y + v0,Z + w0,2));
          if (is_oriented_only && u*pu + v*pv + w*pw<0) { u = -u; v = -v; w = -w; }
          if (is_backward_tracking) { X-=(pu=u); Y-=(pv=v); Z-=(pw=w); } else { X+=(pu=u); Y+=(pv=v); Z+=(pw=w); }
          if (is_bounded && (X<x0 || X>x1 || Y<y0 || Y>y1 || Z<z0 || Z>z1)) break;
        }
      } break;
      default : { // Fourth order interpolation.
        cimg_forX(coordinates,x) {
          *(ptr_x++) = X; *(ptr_y++) = Y; *(ptr_z++) = Z;
          float
            u0 = (float)(dl2*func(X,Y,Z,0)),
            v0 = (float)(dl2*func(X,Y,Z,1)),
            w0 = (float)(dl2*func(X,Y,Z,2));
          if (is_oriented_only && u0*pu + v0*pv + w0*pw<0) { u0 = -u0; v0 = -v0; w0 = -w0; }
          float
            u1 = (float)(dl2*func(X + u0,Y + v0,Z + w0,0)),
            v1 = (float)(dl2*func(X + u0,Y + v0,Z + w0,1)),
            w1 = (float)(dl2*func(X + u0,Y + v0,Z + w0,2));
          if (is_oriented_only && u1*pu + v1*pv + w1*pw<0) { u1 = -u1; v1 = -v1; w1 = -w1; }
          float
            u2 = (float)(dl2*func(X + u1,Y + v1,Z + w1,0)),
            v2 = (float)(dl2*func(X + u1,Y + v1,Z + w1,1)),
            w2 = (float)(dl2*func(X + u1,Y + v1,Z + w1,2));
          if (is_oriented_only && u2*pu + v2*pv + w2*pw<0) { u2 = -u2; v2 = -v2; w2 = -w2; }
          float
            u3 = (float)(dl2*func(X + u2,Y + v2,Z + w2,0)),
            v3 = (float)(dl2*func(X + u2,Y + v2,Z + w2,1)),
            w3 = (float)(dl2*func(X + u2,Y + v2,Z + w2,2));
          if (is_oriented_only && u2*pu + v2*pv + w2*pw<0) { u3 = -u3; v3 = -v3; w3 = -w3; }
          const float
            u = (u0 + u3)/3 + (u1 + u2)/1.5f,
            v = (v0 + v3)/3 + (v1 + v2)/1.5f,
            w = (w0 + w3)/3 + (w1 + w2)/1.5f;
          if (is_backward_tracking) { X-=(pu=u); Y-=(pv=v); Z-=(pw=w); } else { X+=(pu=u); Y+=(pv=v); Z+=(pw=w); }
          if (is_bounded && (X<x0 || X>x1 || Y<y0 || Y>y1 || Z<z0 || Z>z1)) break;
        }
      }
      }
      if (ptr_x!=coordinates.data(0,1)) coordinates.resize((int)(ptr_x-coordinates.data()),3,1,1,0);
      return coordinates;