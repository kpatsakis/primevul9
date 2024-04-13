    //! Select sub-graph in a graph.
    CImg<intT> get_select_graph(CImgDisplay &disp,
                                const unsigned int plot_type=1, const unsigned int vertex_type=1,
                                const char *const labelx=0, const double xmin=0, const double xmax=0,
                                const char *const labely=0, const double ymin=0, const double ymax=0,
                                const bool exit_on_anykey=false) const {
      if (is_empty())
        throw CImgInstanceException(_cimg_instance
                                    "select_graph(): Empty instance.",
                                    cimg_instance);
      if (!disp) disp.assign(cimg_fitscreen(CImgDisplay::screen_width()/2,CImgDisplay::screen_height()/2,1),0,0).
                   set_title("CImg<%s>",pixel_type());
      const ulongT siz = (ulongT)_width*_height*_depth;
      const unsigned int old_normalization = disp.normalization();
      disp.show().set_button().set_wheel()._normalization = 0;

      double nymin = ymin, nymax = ymax, nxmin = xmin, nxmax = xmax;
      if (nymin==nymax) { nymin = (Tfloat)min_max(nymax); const double dy = nymax - nymin; nymin-=dy/20; nymax+=dy/20; }
      if (nymin==nymax) { --nymin; ++nymax; }
      if (nxmin==nxmax && nxmin==0) { nxmin = 0; nxmax = siz - 1.0; }

      static const unsigned char black[] = { 0, 0, 0 }, white[] = { 255, 255, 255 }, gray[] = { 220, 220, 220 };
      static const unsigned char gray2[] = { 110, 110, 110 }, ngray[] = { 35, 35, 35 };
      static unsigned int odimv = 0;
      static CImg<ucharT> colormap;
      if (odimv!=_spectrum) {
        odimv = _spectrum;
        colormap = CImg<ucharT>(3,_spectrum,1,1,120).noise(70,1);
        if (_spectrum==1) { colormap[0] = colormap[1] = 120; colormap[2] = 200; }
        else {
          colormap(0,0) = 220; colormap(1,0) = 10; colormap(2,0) = 10;
          if (_spectrum>1) { colormap(0,1) = 10; colormap(1,1) = 220; colormap(2,1) = 10; }
          if (_spectrum>2) { colormap(0,2) = 10; colormap(1,2) = 10; colormap(2,2) = 220; }
        }
      }

      CImg<ucharT> visu0, visu, graph, text, axes;
      int x0 = -1, x1 = -1, y0 = -1, y1 = -1, omouse_x = -2, omouse_y = -2;
      const unsigned int one = plot_type==3?0U:1U;
      unsigned int okey = 0, obutton = 0;
      CImg<charT> message(1024);
      CImg_3x3(I,unsigned char);

      for (bool selected = false; !selected && !disp.is_closed() && !okey && !disp.wheel(); ) {
        const int mouse_x = disp.mouse_x(), mouse_y = disp.mouse_y();
        const unsigned int key = disp.key(), button = disp.button();

        // Generate graph representation.
        if (!visu0) {
          visu0.assign(disp.width(),disp.height(),1,3,220);
          const int gdimx = disp.width() - 32, gdimy = disp.height() - 32;
          if (gdimx>0 && gdimy>0) {
            graph.assign(gdimx,gdimy,1,3,255);
            if (siz<32) {
              if (siz>1) graph.draw_grid(gdimx/(float)(siz - one),gdimy/(float)(siz - one),0,0,
                                         false,true,black,0.2f,0x33333333,0x33333333);
            } else graph.draw_grid(-10,-10,0,0,false,true,black,0.2f,0x33333333,0x33333333);
            cimg_forC(*this,c)
              graph.draw_graph(get_shared_channel(c),&colormap(0,c),(plot_type!=3 || _spectrum==1)?1:0.6f,
                               plot_type,vertex_type,nymax,nymin);

            axes.assign(gdimx,gdimy,1,1,0);
            const float
              dx = (float)cimg::abs(nxmax - nxmin), dy = (float)cimg::abs(nymax - nymin),
              px = (float)std::pow(10.0,(int)std::log10(dx?dx:1) - 2.0),
              py = (float)std::pow(10.0,(int)std::log10(dy?dy:1) - 2.0);
            const CImg<Tdouble>
              seqx = dx<=0?CImg<Tdouble>::vector(nxmin):
                CImg<Tdouble>::sequence(1 + gdimx/60,nxmin,one?nxmax:nxmin + (nxmax - nxmin)*(siz + 1)/siz).round(px),
              seqy = CImg<Tdouble>::sequence(1 + gdimy/60,nymax,nymin).round(py);

            const bool allow_zero = (nxmin*nxmax>0) || (nymin*nymax>0);
            axes.draw_axes(seqx,seqy,white,1,~0U,~0U,13,allow_zero);
            if (nymin>0) axes.draw_axis(seqx,gdimy - 1,gray,1,~0U,13,allow_zero);
            if (nymax<0) axes.draw_axis(seqx,0,gray,1,~0U,13,allow_zero);
	    if (nxmin>0) axes.draw_axis(0,seqy,gray,1,~0U,13,allow_zero);
	    if (nxmax<0) axes.draw_axis(gdimx - 1,seqy,gray,1,~0U,13,allow_zero);

            cimg_for3x3(axes,x,y,0,0,I,unsigned char)
              if (Icc) {
                if (Icc==255) cimg_forC(graph,c) graph(x,y,c) = 0;
                else cimg_forC(graph,c) graph(x,y,c) = (unsigned char)(2*graph(x,y,c)/3);
              }
              else if (Ipc || Inc || Icp || Icn || Ipp || Inn || Ipn || Inp)
                cimg_forC(graph,c) graph(x,y,c) = (unsigned char)((graph(x,y,c) + 511)/3);

            visu0.draw_image(16,16,graph);
	    visu0.draw_line(15,15,16 + gdimx,15,gray2).draw_line(16 + gdimx,15,16 + gdimx,16 + gdimy,gray2).
	      draw_line(16 + gdimx,16 + gdimy,15,16 + gdimy,white).draw_line(15,16 + gdimy,15,15,white);
          } else graph.assign();
          text.assign().draw_text(0,0,labelx?labelx:"X-axis",white,ngray,1,13).resize(-100,-100,1,3);
          visu0.draw_image((visu0.width() - text.width())/2,visu0.height() - 14,~text);
          text.assign().draw_text(0,0,labely?labely:"Y-axis",white,ngray,1,13).rotate(-90).resize(-100,-100,1,3);
          visu0.draw_image(1,(visu0.height() - text.height())/2,~text);
          visu.assign();
        }

        // Generate and display current view.
        if (!visu) {
          visu.assign(visu0);
          if (graph && x0>=0 && x1>=0) {
            const int
              nx0 = x0<=x1?x0:x1,
              nx1 = x0<=x1?x1:x0,
              ny0 = y0<=y1?y0:y1,
              ny1 = y0<=y1?y1:y0,
              sx0 = (int)(16 + nx0*(visu.width() - 32)/std::max((ulongT)1,siz - one)),
              sx1 = (int)(15 + (nx1 + 1)*(visu.width() - 32)/std::max((ulongT)1,siz - one)),
              sy0 = 16 + ny0,
              sy1 = 16 + ny1;
            if (y0>=0 && y1>=0)
              visu.draw_rectangle(sx0,sy0,sx1,sy1,gray,0.5f).draw_rectangle(sx0,sy0,sx1,sy1,black,0.5f,0xCCCCCCCCU);
            else visu.draw_rectangle(sx0,0,sx1,visu.height() - 17,gray,0.5f).
                   draw_line(sx0,16,sx0,visu.height() - 17,black,0.5f,0xCCCCCCCCU).
                   draw_line(sx1,16,sx1,visu.height() - 17,black,0.5f,0xCCCCCCCCU);
          }
          if (mouse_x>=16 && mouse_y>=16 && mouse_x<visu.width() - 16 && mouse_y<visu.height() - 16) {
            if (graph) visu.draw_line(mouse_x,16,mouse_x,visu.height() - 17,black,0.5f,0x55555555U);
            const unsigned int
              x = (unsigned int)cimg::round((mouse_x - 16.0f)*(siz - one)/(disp.width() - 32),1,one?0:-1);
            const double cx = nxmin + x*(nxmax - nxmin)/std::max((ulongT)1,siz - 1);
            if (_spectrum>=7)
              cimg_snprintf(message,message._width,"Value[%u:%g] = ( %g %g %g ... %g %g %g )",x,cx,
                            (double)(*this)(x,0,0,0),(double)(*this)(x,0,0,1),(double)(*this)(x,0,0,2),
                            (double)(*this)(x,0,0,_spectrum - 4),(double)(*this)(x,0,0,_spectrum - 3),
                            (double)(*this)(x,0,0,_spectrum - 1));
            else {
              cimg_snprintf(message,message._width,"Value[%u:%g] = ( ",x,cx);
              cimg_forC(*this,c) cimg_sprintf(message._data + std::strlen(message),"%g ",(double)(*this)(x,0,0,c));
              cimg_sprintf(message._data + std::strlen(message),")");
            }
	    if (x0>=0 && x1>=0) {
	      const unsigned int
                nx0 = (unsigned int)(x0<=x1?x0:x1),
                nx1 = (unsigned int)(x0<=x1?x1:x0),
                ny0 = (unsigned int)(y0<=y1?y0:y1),
                ny1 = (unsigned int)(y0<=y1?y1:y0);
	      const double
                cx0 = nxmin + nx0*(nxmax - nxmin)/std::max((ulongT)1,siz - 1),
                cx1 = nxmin + (nx1 + one)*(nxmax - nxmin)/std::max((ulongT)1,siz - 1),
                cy0 = nymax - ny0*(nymax - nymin)/(visu._height - 32),
                cy1 = nymax - ny1*(nymax - nymin)/(visu._height - 32);
	      if (y0>=0 && y1>=0)
	        cimg_sprintf(message._data + std::strlen(message)," - Range ( %u:%g, %g ) - ( %u:%g, %g )",
                             x0,cx0,cy0,x1 + one,cx1,cy1);
	      else
	        cimg_sprintf(message._data + std::strlen(message)," - Range [ %u:%g - %u:%g ]",
                             x0,cx0,x1 + one,cx1);
	    }
            text.assign().draw_text(0,0,message,white,ngray,1,13).resize(-100,-100,1,3);
            visu.draw_image((visu.width() - text.width())/2,1,~text);
          }
          visu.display(disp);
        }

        // Test keys.
        CImg<charT> filename(32);
        switch (okey = key) {
#if cimg_OS!=2
        case cimg::keyCTRLRIGHT : case cimg::keySHIFTRIGHT :
#endif
        case cimg::keyCTRLLEFT : case cimg::keySHIFTLEFT : okey = 0; break;
        case cimg::keyD : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
          disp.set_fullscreen(false).
            resize(CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,false),
                   CImgDisplay::_fitscreen(3*disp.width()/2,3*disp.height()/2,1,128,-100,true),false).
            _is_resized = true;
          disp.set_key(key,false); okey = 0;
        } break;
        case cimg::keyC : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
          disp.set_fullscreen(false).
            resize(cimg_fitscreen(2*disp.width()/3,2*disp.height()/3,1),false)._is_resized = true;
          disp.set_key(key,false); okey = 0;
        } break;
        case cimg::keyR : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.set_fullscreen(false).
              resize(cimg_fitscreen(CImgDisplay::screen_width()/2,
                                    CImgDisplay::screen_height()/2,1),false)._is_resized = true;
            disp.set_key(key,false); okey = 0;
          } break;
        case cimg::keyF : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            disp.resize(disp.screen_width(),disp.screen_height(),false).toggle_fullscreen()._is_resized = true;
            disp.set_key(key,false); okey = 0;
          } break;
        case cimg::keyS : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            static unsigned int snap_number = 0;
            if (visu || visu0) {
              CImg<ucharT> &screen = visu?visu:visu0;
              std::FILE *file;
              do {
                cimg_snprintf(filename,filename._width,cimg_appname "_%.4u.bmp",snap_number++);
                if ((file=std_fopen(filename,"r"))!=0) cimg::fclose(file);
              } while (file);
              (+screen).draw_text(0,0," Saving snapshot... ",black,gray,1,13).display(disp);
              screen.save(filename);
              (+screen).draw_text(0,0," Snapshot '%s' saved. ",black,gray,1,13,filename._data).display(disp);
            }
            disp.set_key(key,false); okey = 0;
          } break;
        case cimg::keyO : if (disp.is_keyCTRLLEFT() || disp.is_keyCTRLRIGHT()) {
            static unsigned int snap_number = 0;
            if (visu || visu0) {
              CImg<ucharT> &screen = visu?visu:visu0;
              std::FILE *file;
              do {
#ifdef cimg_use_zlib
                cimg_snprintf(filename,filename._width,cimg_appname "_%.4u.cimgz",snap_number++);
#else
                cimg_snprintf(filename,filename._width,cimg_appname "_%.4u.cimg",snap_number++);
#endif
                if ((file=std_fopen(filename,"r"))!=0) cimg::fclose(file);
              } while (file);
              (+screen).draw_text(0,0," Saving instance... ",black,gray,1,13).display(disp);
              save(filename);
              (+screen).draw_text(0,0," Instance '%s' saved. ",black,gray,1,13,filename._data).display(disp);
            }
            disp.set_key(key,false); okey = 0;
          } break;
        }

        // Handle mouse motion and mouse buttons
        if (obutton!=button || omouse_x!=mouse_x || omouse_y!=mouse_y) {
          visu.assign();
          if (disp.mouse_x()>=0 && disp.mouse_y()>=0) {
            const int
              mx = (mouse_x - 16)*(int)(siz - one)/(disp.width() - 32),
              cx = cimg::cut(mx,0,(int)(siz - 1 - one)),
              my = mouse_y - 16,
              cy = cimg::cut(my,0,disp.height() - 32);
	    if (button&1) {
              if (!obutton) { x0 = cx; y0 = -1; } else { x1 = cx; y1 = -1; }
            }
	    else if (button&2) {
              if (!obutton) { x0 = cx; y0 = cy; } else { x1 = cx; y1 = cy; }
            }
            else if (obutton) { x1 = x1>=0?cx:-1; y1 = y1>=0?cy:-1; selected = true; }
          } else if (!button && obutton) selected = true;
          obutton = button; omouse_x = mouse_x; omouse_y = mouse_y;
        }
        if (disp.is_resized()) { disp.resize(false); visu0.assign(); }
        if (visu && visu0) disp.wait();
        if (!exit_on_anykey && okey && okey!=cimg::keyESC &&
            (okey!=cimg::keyW || (!disp.is_keyCTRLLEFT() && !disp.is_keyCTRLRIGHT()))) {
          disp.set_key(key,false);
          okey = 0;
        }
      }

      disp._normalization = old_normalization;
      if (x1>=0 && x1<x0) cimg::swap(x0,x1);
      if (y1<y0) cimg::swap(y0,y1);
      disp.set_key(okey);
      return CImg<intT>(4,1,1,1,x0,y0,x1>=0?x1 + (int)one:-1,y1);