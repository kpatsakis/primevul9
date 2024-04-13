    template<typename tx, typename ty, typename tc>
    CImg<T>& draw_axes(const CImg<tx>& values_x, const CImg<ty>& values_y,
                       const tc *const color, const float opacity=1,
                       const unsigned int pattern_x=~0U, const unsigned int pattern_y=~0U,
                       const unsigned int font_height=13, const bool allow_zero=true) {
      if (is_empty()) return *this;
      const CImg<tx> nvalues_x(values_x._data,values_x.size(),1,1,1,true);
      const int sizx = (int)values_x.size() - 1, wm1 = width() - 1;
      if (sizx>=0) {
        float ox = (float)*nvalues_x;
        for (unsigned int x = sizx?1U:0U; x<_width; ++x) {
          const float nx = (float)nvalues_x._linear_atX((float)x*sizx/wm1);
          if (nx*ox<=0) { draw_axis(nx==0?x:x - 1,values_y,color,opacity,pattern_y,font_height,allow_zero); break; }
          ox = nx;
        }
      }
      const CImg<ty> nvalues_y(values_y._data,values_y.size(),1,1,1,true);
      const int sizy = (int)values_y.size() - 1, hm1 = height() - 1;
      if (sizy>0) {
        float oy = (float)nvalues_y[0];
        for (unsigned int y = sizy?1U:0U; y<_height; ++y) {
          const float ny = (float)nvalues_y._linear_atX((float)y*sizy/hm1);
          if (ny*oy<=0) { draw_axis(values_x,ny==0?y:y - 1,color,opacity,pattern_x,font_height,allow_zero); break; }
          oy = ny;
        }
      }
      return *this;