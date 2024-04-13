    template<typename t, typename tc>
    CImg<T>& draw_axis(const CImg<t>& values_x, const int y,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const unsigned int font_height=13,
                       const bool allow_zero=true) {
      if (is_empty()) return *this;
      const int yt = (y + 3 + font_height)<_height?y + 3:y - 2 - (int)font_height;
      const int siz = (int)values_x.size() - 1;
      CImg<charT> txt(32);
      CImg<T> label;
      if (siz<=0) { // Degenerated case.
        draw_line(0,y,_width - 1,y,color,opacity,pattern);
        if (!siz) {
          cimg_snprintf(txt,txt._width,"%g",(double)*values_x);
          label.assign().draw_text(0,0,txt,color,(tc*)0,opacity,font_height);
          const int
            _xt = (width() - label.width())/2,
            xt = _xt<3?3:_xt + label.width()>=width() - 2?width() - 3 - label.width():_xt;
          draw_point(width()/2,y - 1,color,opacity).draw_point(width()/2,y + 1,color,opacity);
          if (allow_zero || *txt!='0' || txt[1]!=0)
            draw_text(xt,yt,txt,color,(tc*)0,opacity,font_height);
        }
      } else { // Regular case.
        if (values_x[0]<values_x[siz]) draw_arrow(0,y,_width - 1,y,color,opacity,30,5,pattern);
        else draw_arrow(_width - 1,y,0,y,color,opacity,30,5,pattern);
        cimg_foroff(values_x,x) {
          cimg_snprintf(txt,txt._width,"%g",(double)values_x(x));
          label.assign().draw_text(0,0,txt,color,(tc*)0,opacity,font_height);
          const int
            xi = (int)(x*(_width - 1)/siz),
            _xt = xi - label.width()/2,
            xt = _xt<3?3:_xt + label.width()>=width() - 2?width() - 3 - label.width():_xt;
          draw_point(xi,y - 1,color,opacity).draw_point(xi,y + 1,color,opacity);
          if (allow_zero || *txt!='0' || txt[1]!=0)
            draw_text(xt,yt,txt,color,(tc*)0,opacity,font_height);
        }
      }
      return *this;