
    Tfloat _cubic_atXY(const float fx, const float fy, const int z=0, const int c=0) const {
      const float
        nfx = cimg::cut(fx,0,width() - 1),
        nfy = cimg::cut(fy,0,height() - 1);
      const int x = (int)nfx, y = (int)nfy;
      const float dx = nfx - x, dy = nfy - y;
      const int
        px = x - 1<0?0:x - 1, nx = dx>0?x + 1:x, ax = x + 2>=width()?width() - 1:x + 2,
        py = y - 1<0?0:y - 1, ny = dy>0?y + 1:y, ay = y + 2>=height()?height() - 1:y + 2;
      const Tfloat
        Ipp = (Tfloat)(*this)(px,py,z,c), Icp = (Tfloat)(*this)(x,py,z,c), Inp = (Tfloat)(*this)(nx,py,z,c),
        Iap = (Tfloat)(*this)(ax,py,z,c),
        Ip = Icp + 0.5f*(dx*(-Ipp + Inp) + dx*dx*(2*Ipp - 5*Icp + 4*Inp - Iap) + dx*dx*dx*(-Ipp + 3*Icp - 3*Inp + Iap)),
        Ipc = (Tfloat)(*this)(px,y,z,c),  Icc = (Tfloat)(*this)(x, y,z,c), Inc = (Tfloat)(*this)(nx,y,z,c),
        Iac = (Tfloat)(*this)(ax,y,z,c),
        Ic = Icc + 0.5f*(dx*(-Ipc + Inc) + dx*dx*(2*Ipc - 5*Icc + 4*Inc - Iac) + dx*dx*dx*(-Ipc + 3*Icc - 3*Inc + Iac)),
        Ipn = (Tfloat)(*this)(px,ny,z,c), Icn = (Tfloat)(*this)(x,ny,z,c), Inn = (Tfloat)(*this)(nx,ny,z,c),
        Ian = (Tfloat)(*this)(ax,ny,z,c),
        In = Icn + 0.5f*(dx*(-Ipn + Inn) + dx*dx*(2*Ipn - 5*Icn + 4*Inn - Ian) + dx*dx*dx*(-Ipn + 3*Icn - 3*Inn + Ian)),
        Ipa = (Tfloat)(*this)(px,ay,z,c), Ica = (Tfloat)(*this)(x,ay,z,c), Ina = (Tfloat)(*this)(nx,ay,z,c),
        Iaa = (Tfloat)(*this)(ax,ay,z,c),
        Ia = Ica + 0.5f*(dx*(-Ipa + Ina) + dx*dx*(2*Ipa - 5*Ica + 4*Ina - Iaa) + dx*dx*dx*(-Ipa + 3*Ica - 3*Ina + Iaa));
      return Ic + 0.5f*(dy*(-Ip + In) + dy*dy*(2*Ip - 5*Ic + 4*In - Ia) + dy*dy*dy*(-Ip + 3*Ic - 3*In + Ia));