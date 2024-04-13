     - If a button text is set to 0, the corresponding button (and the followings) will not appear in the dialog box.
     At least one button must be specified.
  **/
  template<typename t>
  inline int dialog(const char *const title, const char *const msg,
                    const char *const button1_label, const char *const button2_label,
                    const char *const button3_label, const char *const button4_label,
                    const char *const button5_label, const char *const button6_label,
                    const CImg<t>& logo, const bool is_centered=false) {
#if cimg_display==0
    cimg::unused(title,msg,button1_label,button2_label,button3_label,button4_label,button5_label,button6_label,
                 logo._data,is_centered);
    throw CImgIOException("cimg::dialog(): No display available.");
#else
    static const unsigned char
      black[] = { 0,0,0 }, white[] = { 255,255,255 }, gray[] = { 200,200,200 }, gray2[] = { 150,150,150 };

    // Create buttons and canvas graphics
    CImgList<unsigned char> buttons, cbuttons, sbuttons;
    if (button1_label) { CImg<unsigned char>().draw_text(0,0,button1_label,black,gray,1,13).move_to(buttons);
      if (button2_label) { CImg<unsigned char>().draw_text(0,0,button2_label,black,gray,1,13).move_to(buttons);
        if (button3_label) { CImg<unsigned char>().draw_text(0,0,button3_label,black,gray,1,13).move_to(buttons);
          if (button4_label) { CImg<unsigned char>().draw_text(0,0,button4_label,black,gray,1,13).move_to(buttons);
            if (button5_label) { CImg<unsigned char>().draw_text(0,0,button5_label,black,gray,1,13).move_to(buttons);
              if (button6_label) { CImg<unsigned char>().draw_text(0,0,button6_label,black,gray,1,13).move_to(buttons);
              }}}}}}
    if (!buttons._width)
      throw CImgArgumentException("cimg::dialog(): No buttons have been defined.");
    cimglist_for(buttons,l) buttons[l].resize(-100,-100,1,3);

    unsigned int bw = 0, bh = 0;
    cimglist_for(buttons,l) { bw = std::max(bw,buttons[l]._width); bh = std::max(bh,buttons[l]._height); }
    bw+=8; bh+=8;
    if (bw<64) bw = 64;
    if (bw>128) bw = 128;
    if (bh<24) bh = 24;
    if (bh>48) bh = 48;

    CImg<unsigned char> button(bw,bh,1,3);
    button.draw_rectangle(0,0,bw - 1,bh - 1,gray);
    button.draw_line(0,0,bw - 1,0,white).draw_line(0,bh - 1,0,0,white);
    button.draw_line(bw - 1,0,bw - 1,bh - 1,black).draw_line(bw - 1,bh - 1,0,bh - 1,black);
    button.draw_line(1,bh - 2,bw - 2,bh - 2,gray2).draw_line(bw - 2,bh - 2,bw - 2,1,gray2);
    CImg<unsigned char> sbutton(bw,bh,1,3);
    sbutton.draw_rectangle(0,0,bw - 1,bh - 1,gray);
    sbutton.draw_line(0,0,bw - 1,0,black).draw_line(bw - 1,0,bw - 1,bh - 1,black);
    sbutton.draw_line(bw - 1,bh - 1,0,bh - 1,black).draw_line(0,bh - 1,0,0,black);
    sbutton.draw_line(1,1,bw - 2,1,white).draw_line(1,bh - 2,1,1,white);
    sbutton.draw_line(bw - 2,1,bw - 2,bh - 2,black).draw_line(bw - 2,bh - 2,1,bh - 2,black);
    sbutton.draw_line(2,bh - 3,bw - 3,bh - 3,gray2).draw_line(bw - 3,bh - 3,bw - 3,2,gray2);
    sbutton.draw_line(4,4,bw - 5,4,black,1,0xAAAAAAAA,true).draw_line(bw - 5,4,bw - 5,bh - 5,black,1,0xAAAAAAAA,false);
    sbutton.draw_line(bw - 5,bh - 5,4,bh - 5,black,1,0xAAAAAAAA,false).draw_line(4,bh - 5,4,4,black,1,0xAAAAAAAA,false);
    CImg<unsigned char> cbutton(bw,bh,1,3);
    cbutton.draw_rectangle(0,0,bw - 1,bh - 1,black).draw_rectangle(1,1,bw - 2,bh - 2,gray2).
      draw_rectangle(2,2,bw - 3,bh - 3,gray);
    cbutton.draw_line(4,4,bw - 5,4,black,1,0xAAAAAAAA,true).draw_line(bw - 5,4,bw - 5,bh - 5,black,1,0xAAAAAAAA,false);
    cbutton.draw_line(bw - 5,bh - 5,4,bh - 5,black,1,0xAAAAAAAA,false).draw_line(4,bh - 5,4,4,black,1,0xAAAAAAAA,false);

    cimglist_for(buttons,ll) {
      CImg<unsigned char>(cbutton).
        draw_image(1 + (bw  -buttons[ll].width())/2,1 + (bh - buttons[ll].height())/2,buttons[ll]).
        move_to(cbuttons);
      CImg<unsigned char>(sbutton).
        draw_image((bw - buttons[ll].width())/2,(bh - buttons[ll].height())/2,buttons[ll]).
        move_to(sbuttons);
      CImg<unsigned char>(button).
        draw_image((bw - buttons[ll].width())/2,(bh - buttons[ll].height())/2,buttons[ll]).
        move_to(buttons[ll]);
    }

    CImg<unsigned char> canvas;
    if (msg)
      ((CImg<unsigned char>().draw_text(0,0,"%s",gray,0,1,13,msg)*=-1)+=200).resize(-100,-100,1,3).move_to(canvas);

    const unsigned int
      bwall = (buttons._width - 1)*(12 + bw) + bw,
      w = cimg::max(196U,36 + logo._width + canvas._width,24 + bwall),
      h = cimg::max(96U,36 + canvas._height + bh,36 + logo._height + bh),
      lx = 12 + (canvas._data?0:((w - 24 - logo._width)/2)),
      ly = (h - 12 - bh - logo._height)/2,
      tx = lx + logo._width + 12,
      ty = (h - 12 - bh - canvas._height)/2,
      bx = (w - bwall)/2,
      by = h - 12 - bh;

    if (canvas._data)
      canvas = CImg<unsigned char>(w,h,1,3).
        draw_rectangle(0,0,w - 1,h - 1,gray).
        draw_line(0,0,w - 1,0,white).draw_line(0,h - 1,0,0,white).
        draw_line(w - 1,0,w - 1,h - 1,black).draw_line(w - 1,h - 1,0,h - 1,black).
        draw_image(tx,ty,canvas);
    else
      canvas = CImg<unsigned char>(w,h,1,3).
        draw_rectangle(0,0,w - 1,h - 1,gray).
        draw_line(0,0,w - 1,0,white).draw_line(0,h - 1,0,0,white).
        draw_line(w - 1,0,w - 1,h - 1,black).draw_line(w - 1,h - 1,0,h - 1,black);
    if (logo._data) canvas.draw_image(lx,ly,logo);

    unsigned int xbuttons[6] = { 0 };
    cimglist_for(buttons,lll) { xbuttons[lll] = bx + (bw + 12)*lll; canvas.draw_image(xbuttons[lll],by,buttons[lll]); }

    // Open window and enter events loop
    CImgDisplay disp(canvas,title?title:" ",0,false,is_centered?true:false);
    if (is_centered) disp.move((CImgDisplay::screen_width() - disp.width())/2,
                               (CImgDisplay::screen_height() - disp.height())/2);
    bool stop_flag = false, refresh = false;
    int oselected = -1, oclicked = -1, selected = -1, clicked = -1;
    while (!disp.is_closed() && !stop_flag) {
      if (refresh) {
        if (clicked>=0)
          CImg<unsigned char>(canvas).draw_image(xbuttons[clicked],by,cbuttons[clicked]).display(disp);
        else {
          if (selected>=0)
            CImg<unsigned char>(canvas).draw_image(xbuttons[selected],by,sbuttons[selected]).display(disp);
          else canvas.display(disp);
        }
        refresh = false;
      }
      disp.wait(15);
      if (disp.is_resized()) disp.resize(disp,false);

      if (disp.button()&1)  {
        oclicked = clicked;
        clicked = -1;
        cimglist_for(buttons,l)
          if (disp.mouse_y()>=(int)by && disp.mouse_y()<(int)(by + bh) &&
              disp.mouse_x()>=(int)xbuttons[l] && disp.mouse_x()<(int)(xbuttons[l] + bw)) {
            clicked = selected = l;
            refresh = true;
          }
        if (clicked!=oclicked) refresh = true;
      } else if (clicked>=0) stop_flag = true;

      if (disp.key()) {
        oselected = selected;
        switch (disp.key()) {
        case cimg::keyESC : selected = -1; stop_flag = true; break;
        case cimg::keyENTER : if (selected<0) selected = 0; stop_flag = true; break;
        case cimg::keyTAB :
        case cimg::keyARROWRIGHT :
        case cimg::keyARROWDOWN : selected = (selected + 1)%buttons.width(); break;
        case cimg::keyARROWLEFT :
        case cimg::keyARROWUP : selected = (selected + buttons.width() - 1)%buttons.width(); break;
        }
        disp.set_key();
        if (selected!=oselected) refresh = true;
      }
    }