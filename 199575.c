    template<typename tc>
    CImg<T>& draw_axes(const float x0, const float x1, const float y0, const float y1,
                       const tc *const color, const float opacity=1,
                       const int subdivisionx=-60, const int subdivisiony=-60,
                       const float precisionx=0, const float precisiony=0,
                       const unsigned int pattern_x=~0U, const unsigned int pattern_y=~0U,
                       const unsigned int font_height=13) {
      if (is_empty()) return *this;
      const bool allow_zero = (x0*x1>0) || (y0*y1>0);
      const float
        dx = cimg::abs(x1 - x0), dy = cimg::abs(y1 - y0),
        px = dx<=0?1:precisionx==0?(float)std::pow(10.0,(int)std::log10(dx) - 2.0):precisionx,
        py = dy<=0?1:precisiony==0?(float)std::pow(10.0,(int)std::log10(dy) - 2.0):precisiony;
      if (x0!=x1 && y0!=y1)
        draw_axes(CImg<floatT>::sequence(subdivisionx>0?subdivisionx:1-width()/subdivisionx,x0,x1).round(px),
                  CImg<floatT>::sequence(subdivisiony>0?subdivisiony:1-height()/subdivisiony,y0,y1).round(py),
                  color,opacity,pattern_x,pattern_y,font_height,allow_zero);
      else if (x0==x1 && y0!=y1)
        draw_axis((int)x0,CImg<floatT>::sequence(subdivisiony>0?subdivisiony:1-height()/subdivisiony,y0,y1).round(py),
                  color,opacity,pattern_y,font_height);
      else if (x0!=x1 && y0==y1)
        draw_axis(CImg<floatT>::sequence(subdivisionx>0?subdivisionx:1-width()/subdivisionx,x0,x1).round(px),(int)y0,
                  color,opacity,pattern_x,font_height);
      return *this;