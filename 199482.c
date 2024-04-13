    **/
    T& atN(const int pos, const int x, const int y, const int z, const int c, const T& out_value) {
      return (pos<0 || pos>=(int)_width)?(cimg::temporary(out_value)=out_value):(*this)(pos,x,y,z,c);