
    static LRESULT APIENTRY _handle_events(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
#ifdef _WIN64
      CImgDisplay *const disp = (CImgDisplay*)GetWindowLongPtr(window,GWLP_USERDATA);
#else
      CImgDisplay *const disp = (CImgDisplay*)GetWindowLong(window,GWL_USERDATA);
#endif
      MSG st_msg;
      switch (msg) {
      case WM_CLOSE :
        disp->_mouse_x = disp->_mouse_y = -1;
        disp->_window_x = disp->_window_y = 0;
        disp->set_button().set_key(0).set_key(0,false)._is_closed = true;
        ReleaseMutex(disp->_mutex);
        ShowWindow(disp->_window,SW_HIDE);
        disp->_is_event = true;
        SetEvent(cimg::Win32_attr().wait_event);
        return 0;
      case WM_SIZE : {
        while (PeekMessage(&st_msg,window,WM_SIZE,WM_SIZE,PM_REMOVE)) {}
        WaitForSingleObject(disp->_mutex,INFINITE);
        const unsigned int nw = LOWORD(lParam),nh = HIWORD(lParam);
        if (nw && nh && (nw!=disp->_width || nh!=disp->_height)) {
          disp->_window_width = nw;
          disp->_window_height = nh;
          disp->_mouse_x = disp->_mouse_y = -1;
          disp->_is_resized = disp->_is_event = true;
          SetEvent(cimg::Win32_attr().wait_event);
        }
        ReleaseMutex(disp->_mutex);
      } break;
      case WM_MOVE : {
        while (PeekMessage(&st_msg,window,WM_SIZE,WM_SIZE,PM_REMOVE)) {}
        WaitForSingleObject(disp->_mutex,INFINITE);
        const int nx = (int)(short)(LOWORD(lParam)), ny = (int)(short)(HIWORD(lParam));
        if (nx!=disp->_window_x || ny!=disp->_window_y) {
          disp->_window_x = nx;
          disp->_window_y = ny;
          disp->_is_moved = disp->_is_event = true;
          SetEvent(cimg::Win32_attr().wait_event);
        }
        ReleaseMutex(disp->_mutex);
      } break;
      case WM_PAINT :
        disp->paint();
        cimg::mutex(15);
        if (disp->_is_cursor_visible) while (ShowCursor(TRUE)<0); else while (ShowCursor(FALSE)>=0);
        cimg::mutex(15,0);
        break;
      case WM_ERASEBKGND :
        //        return 0;
        break;
      case WM_KEYDOWN :
        disp->set_key((unsigned int)wParam);
        SetEvent(cimg::Win32_attr().wait_event);
        break;
      case WM_KEYUP :
        disp->set_key((unsigned int)wParam,false);
        SetEvent(cimg::Win32_attr().wait_event);
        break;
      case WM_MOUSEMOVE : {
        while (PeekMessage(&st_msg,window,WM_MOUSEMOVE,WM_MOUSEMOVE,PM_REMOVE)) {}
        disp->_mouse_x = LOWORD(lParam);
        disp->_mouse_y = HIWORD(lParam);
#if (_WIN32_WINNT>=0x0400) && !defined(NOTRACKMOUSEEVENT)
        if (!disp->_is_mouse_tracked) {
          TRACKMOUSEEVENT tme;
          tme.cbSize = sizeof(TRACKMOUSEEVENT);
          tme.dwFlags = TME_LEAVE;
          tme.hwndTrack = disp->_window;
          if (TrackMouseEvent(&tme)) disp->_is_mouse_tracked = true;
        }
#endif
        if (disp->_mouse_x<0 || disp->_mouse_y<0 || disp->_mouse_x>=disp->width() || disp->_mouse_y>=disp->height())
          disp->_mouse_x = disp->_mouse_y = -1;
        disp->_is_event = true;
        SetEvent(cimg::Win32_attr().wait_event);
        cimg::mutex(15);
	if (disp->_is_cursor_visible) while (ShowCursor(TRUE)<0); else while (ShowCursor(FALSE)>=0);
        cimg::mutex(15,0);
      }	break;
      case WM_MOUSELEAVE : {
        disp->_mouse_x = disp->_mouse_y = -1;
        disp->_is_mouse_tracked = false;
        cimg::mutex(15);
	while (ShowCursor(TRUE)<0) {}
        cimg::mutex(15,0);
      } break;
      case WM_LBUTTONDOWN :
        disp->set_button(1);
        SetEvent(cimg::Win32_attr().wait_event);
        break;
      case WM_RBUTTONDOWN :
        disp->set_button(2);
        SetEvent(cimg::Win32_attr().wait_event);
        break;
      case WM_MBUTTONDOWN :
        disp->set_button(3);
        SetEvent(cimg::Win32_attr().wait_event);
        break;
      case WM_LBUTTONUP :
        disp->set_button(1,false);
        SetEvent(cimg::Win32_attr().wait_event);
        break;
      case WM_RBUTTONUP :
        disp->set_button(2,false);
        SetEvent(cimg::Win32_attr().wait_event);
        break;
      case WM_MBUTTONUP :
        disp->set_button(3,false);
        SetEvent(cimg::Win32_attr().wait_event);
        break;
      case 0x020A : // WM_MOUSEWHEEL:
        disp->set_wheel((int)((short)HIWORD(wParam))/120);
        SetEvent(cimg::Win32_attr().wait_event);
      }
      return DefWindowProc(window,msg,wParam,lParam);