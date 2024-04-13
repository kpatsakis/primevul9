
    static DWORD WINAPI _events_thread(void* arg) {
      CImgDisplay *const disp = (CImgDisplay*)(((void**)arg)[0]);
      const char *const title = (const char*)(((void**)arg)[1]);
      MSG msg;
      delete[] (void**)arg;
      disp->_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
      disp->_bmi.bmiHeader.biWidth = disp->width();
      disp->_bmi.bmiHeader.biHeight = -disp->height();
      disp->_bmi.bmiHeader.biPlanes = 1;
      disp->_bmi.bmiHeader.biBitCount = 32;
      disp->_bmi.bmiHeader.biCompression = BI_RGB;
      disp->_bmi.bmiHeader.biSizeImage = 0;
      disp->_bmi.bmiHeader.biXPelsPerMeter = 1;
      disp->_bmi.bmiHeader.biYPelsPerMeter = 1;
      disp->_bmi.bmiHeader.biClrUsed = 0;
      disp->_bmi.bmiHeader.biClrImportant = 0;
      disp->_data = new unsigned int[(size_t)disp->_width*disp->_height];
      if (!disp->_is_fullscreen) { // Normal window
        RECT rect;
        rect.left = rect.top = 0; rect.right = (LONG)disp->_width - 1; rect.bottom = (LONG)disp->_height - 1;
        AdjustWindowRect(&rect,WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,false);
        const int
          border1 = (int)((rect.right - rect.left + 1 - disp->_width)/2),
          border2 = (int)(rect.bottom - rect.top + 1 - disp->_height - border1);
        disp->_window = CreateWindowA("MDICLIENT",title?title:" ",
                                     WS_OVERLAPPEDWINDOW | (disp->_is_closed?0:WS_VISIBLE), CW_USEDEFAULT,CW_USEDEFAULT,
                                     disp->_width + 2*border1, disp->_height + border1 + border2,
                                     0,0,0,&(disp->_ccs));
        if (!disp->_is_closed) {
          GetWindowRect(disp->_window,&rect);
          disp->_window_x = rect.left + border1;
          disp->_window_y = rect.top + border2;
        } else disp->_window_x = disp->_window_y = 0;
      } else { // Fullscreen window
        const unsigned int
          sx = (unsigned int)screen_width(),
          sy = (unsigned int)screen_height();
        disp->_window = CreateWindowA("MDICLIENT",title?title:" ",
                                     WS_POPUP | (disp->_is_closed?0:WS_VISIBLE),
                                      (sx - disp->_width)/2,
                                      (sy - disp->_height)/2,
                                     disp->_width,disp->_height,0,0,0,&(disp->_ccs));
        disp->_window_x = disp->_window_y = 0;
      }
      SetForegroundWindow(disp->_window);
      disp->_hdc = GetDC(disp->_window);
      disp->_window_width = disp->_width;
      disp->_window_height = disp->_height;
      disp->flush();
#ifdef _WIN64
      SetWindowLongPtr(disp->_window,GWLP_USERDATA,(LONG_PTR)disp);
      SetWindowLongPtr(disp->_window,GWLP_WNDPROC,(LONG_PTR)_handle_events);
#else
      SetWindowLong(disp->_window,GWL_USERDATA,(LONG)disp);
      SetWindowLong(disp->_window,GWL_WNDPROC,(LONG)_handle_events);
#endif
      SetEvent(disp->_is_created);
      while (GetMessage(&msg,0,0,0)) DispatchMessage(&msg);
      return 0;