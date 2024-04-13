
    Tfloat _cubic_atX(const float fx, const int y=0, const int z=0, const int c=0) const {
      const float
        nfx = cimg::cut(fx,0,width() - 1);
      const int
        x = (int)nfx;
      const float
        dx = nfx - x;
      const int
        px = x - 1<0?0:x - 1, nx = dx>0?x + 1:x, ax = x + 2>=width()?width() - 1:x + 2;
      const Tfloat
        Ip = (Tfloat)(*this)(px,y,z,c), Ic = (Tfloat)(*this)(x,y,z,c),
        In = (Tfloat)(*this)(nx,y,z,c), Ia = (Tfloat)(*this)(ax,y,z,c);
      return Ic + 0.5f*(dx*(-Ip + In) + dx*dx*(2*Ip - 5*Ic + 4*In - Ia) + dx*dx*dx*(-Ip + 3*Ic - 3*In + Ia));