                                 CImgDisplay::_fitscreen(dx,dy,dz,128,-85,true)
    static unsigned int _fitscreen(const unsigned int dx, const unsigned int dy, const unsigned int dz,
                                   const int dmin, const int dmax,const bool return_y) {
      const unsigned int _nw = dx + (dz>1?dz:0), _nh = dy + (dz>1?dz:0);
      unsigned int nw = _nw?_nw:1, nh = _nh?_nh:1;
      const unsigned int
        sw = (unsigned int)CImgDisplay::screen_width(),
        sh = (unsigned int)CImgDisplay::screen_height(),
        mw = dmin<0?(unsigned int)(sw*-dmin/100):(unsigned int)dmin,
        mh = dmin<0?(unsigned int)(sh*-dmin/100):(unsigned int)dmin,
        Mw = dmax<0?(unsigned int)(sw*-dmax/100):(unsigned int)dmax,
        Mh = dmax<0?(unsigned int)(sh*-dmax/100):(unsigned int)dmax;
      if (nw<mw) { nh = nh*mw/nw; nh+=(nh==0); nw = mw; }
      if (nh<mh) { nw = nw*mh/nh; nw+=(nw==0); nh = mh; }
      if (nw>Mw) { nh = nh*Mw/nw; nh+=(nh==0); nw = Mw; }
      if (nh>Mh) { nw = nw*Mh/nh; nw+=(nw==0); nh = Mh; }
      if (nw<mw) nw = mw;
      if (nh<mh) nh = mh;
      return return_y?nh:nw;