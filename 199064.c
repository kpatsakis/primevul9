    template<typename t, typename tc>
    CImg<T>& draw_axis(const int x, const CImg<t>& values_y,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern=~0U, const unsigned int font_height=13,
                       const bool allow_zero=true) {
      if (is_empty()) return *this;
      int siz = (int)values_y.size() - 1;
      CImg<charT> txt(32);
      CImg<T> label;
      if (siz<=0) { // Degenerated case.
        draw_line(x,0,x,_height - 1,color,opacity,pattern);
        if (!siz) {
          cimg_snprintf(txt,txt._width,"%g",(double)*values_y);
          label.assign().draw_text(0,0,txt,color,(tc*)0,opacity,font_height);
          const int
            _yt = (height() - label.height())/2,
            yt = _yt<0?0:_yt + label.height()>=height()?height() - 1-label.height():_yt,
            _xt = x - 2 - label.width(),
            xt = _xt>=0?_xt:x + 3;
          draw_point(x - 1,height()/2,color,opacity).draw_point(x + 1,height()/2,color,opacity);
          if (allow_zero || *txt!='0' || txt[1]!=0)
            draw_text(xt,yt,txt,color,(tc*)0,opacity,font_height);
        }
      } else { // Regular case.
        if (values_y[0]<values_y[siz]) draw_arrow(x,0,x,_height - 1,color,opacity,30,5,pattern);
        else draw_arrow(x,_height - 1,x,0,color,opacity,30,5,pattern);
        cimg_foroff(values_y,y) {
          cimg_snprintf(txt,txt._width,"%g",(double)values_y(y));
          label.assign().draw_text(0,0,txt,color,(tc*)0,opacity,font_height);
          const int
            yi = (int)(y*(_height - 1)/siz),
            _yt = yi - label.height()/2,
            yt = _yt<0?0:_yt + label.height()>=height()?height() - 1-label.height():_yt,
            _xt = x - 2 - label.width(),
            xt = _xt>=0?_xt:x + 3;
          draw_point(x - 1,yi,color,opacity).draw_point(x + 1,yi,color,opacity);
          if (allow_zero || *txt!='0' || txt[1]!=0)
            draw_text(xt,yt,txt,color,(tc*)0,opacity,font_height);
        }
      }
      return *this;