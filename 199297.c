    template<typename t, typename tc>
    CImg<T>& draw_graph(const CImg<t>& data,
                        const tc *const color, const float opacity=1,
                        const unsigned int plot_type=1, const int vertex_type=1,
                        const double ymin=0, const double ymax=0, const unsigned int pattern=~0U) {
      if (is_empty() || _height<=1) return *this;
      if (!color)
        throw CImgArgumentException(_cimg_instance
                                    "draw_graph(): Specified color is (null).",
                                    cimg_instance);

      // Create shaded colors for displaying bar plots.
      CImg<tc> color1, color2;
      if (plot_type==3) {
        color1.assign(_spectrum); color2.assign(_spectrum);
        cimg_forC(*this,c) {
          color1[c] = (tc)std::min((float)cimg::type<tc>::max(),(float)color[c]*1.2f);
          color2[c] = (tc)(color[c]*0.4f);
        }
      }

      // Compute min/max and normalization factors.
      const ulongT
        siz = data.size(),
        _siz1 = siz - (plot_type!=3),
        siz1 = _siz1?_siz1:1;
      const unsigned int
        _width1 = _width - (plot_type!=3),
        width1 = _width1?_width1:1;
      double m = ymin, M = ymax;
      if (ymin==ymax) m = (double)data.max_min(M);
      if (m==M) { --m; ++M; }
      const float ca = (float)(M-m)/(_height - 1);
      bool init_hatch = true;

      // Draw graph edges
      switch (plot_type%4) {
      case 1 : { // Segments
        int oX = 0, oY = (int)((data[0] - m)/ca);
        if (siz==1) {
          const int Y = (int)((*data - m)/ca);
          draw_line(0,Y,width() - 1,Y,color,opacity,pattern);
        } else {
          const float fx = (float)_width/siz1;
          for (ulongT off = 1; off<siz; ++off) {
            const int
              X = (int)(off*fx) - 1,
              Y = (int)((data[off]-m)/ca);
            draw_line(oX,oY,X,Y,color,opacity,pattern,init_hatch);
            oX = X; oY = Y;
            init_hatch = false;
          }
        }
      } break;
      case 2 : { // Spline
        const CImg<t> ndata(data._data,siz,1,1,1,true);
        int oY = (int)((data[0] - m)/ca);
        cimg_forX(*this,x) {
          const int Y = (int)((ndata._cubic_atX((float)x*siz1/width1)-m)/ca);
          if (x>0) draw_line(x,oY,x + 1,Y,color,opacity,pattern,init_hatch);
          init_hatch = false;
          oY = Y;
        }
      } break;
      case 3 : { // Bars
        const int Y0 = (int)(-m/ca);
        const float fx = (float)_width/siz1;
        int oX = 0;
        cimg_foroff(data,off) {
          const int
            X = (int)((off + 1)*fx) - 1,
            Y = (int)((data[off] - m)/ca);
          draw_rectangle(oX,Y0,X,Y,color,opacity).
            draw_line(oX,Y,oX,Y0,color2.data(),opacity).
            draw_line(oX,Y0,X,Y0,Y<=Y0?color2.data():color1.data(),opacity).
            draw_line(X,Y,X,Y0,color1.data(),opacity).
            draw_line(oX,Y,X,Y,Y<=Y0?color1.data():color2.data(),opacity);
          oX = X + 1;
        }
      } break;
      default : break; // No edges
      }

      // Draw graph points
      const unsigned int wb2 = plot_type==3?_width1/(2*siz):0;
      const float fx = (float)_width1/siz1;
      switch (vertex_type%8) {
      case 1 : { // Point
        cimg_foroff(data,off) {
          const int
            X = (int)(off*fx + wb2),
            Y = (int)((data[off]-m)/ca);
          draw_point(X,Y,color,opacity);
        }
      } break;
      case 2 : { // Straight Cross
        cimg_foroff(data,off) {
          const int
            X = (int)(off*fx + wb2),
            Y = (int)((data[off]-m)/ca);
          draw_line(X - 3,Y,X + 3,Y,color,opacity).draw_line(X,Y - 3,X,Y + 3,color,opacity);
        }
      } break;
      case 3 : { // Diagonal Cross
        cimg_foroff(data,off) {
          const int
            X = (int)(off*fx + wb2),
            Y = (int)((data[off]-m)/ca);
          draw_line(X - 3,Y - 3,X + 3,Y + 3,color,opacity).draw_line(X - 3,Y + 3,X + 3,Y - 3,color,opacity);
        }
      } break;
      case 4 : { // Filled Circle
        cimg_foroff(data,off) {
          const int
            X = (int)(off*fx + wb2),
            Y = (int)((data[off]-m)/ca);
          draw_circle(X,Y,3,color,opacity);
        }
      } break;
      case 5 : { // Outlined circle
        cimg_foroff(data,off) {
          const int
            X = (int)(off*fx + wb2),
            Y = (int)((data[off]-m)/ca);
          draw_circle(X,Y,3,color,opacity,0U);
        }
      } break;
      case 6 : { // Square
        cimg_foroff(data,off) {
          const int
            X = (int)(off*fx + wb2),
            Y = (int)((data[off]-m)/ca);
          draw_rectangle(X - 3,Y - 3,X + 3,Y + 3,color,opacity,~0U);
        }
      } break;
      case 7 : { // Diamond
        cimg_foroff(data,off) {
          const int
            X = (int)(off*fx + wb2),
            Y = (int)((data[off]-m)/ca);
          draw_line(X,Y - 4,X + 4,Y,color,opacity).
            draw_line(X + 4,Y,X,Y + 4,color,opacity).
            draw_line(X,Y + 4,X - 4,Y,color,opacity).
            draw_line(X - 4,Y,X,Y - 4,color,opacity);
        }
      } break;
      default : break; // No points
      }
      return *this;