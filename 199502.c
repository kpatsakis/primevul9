
    inline unsigned int _wait(const unsigned int milliseconds, cimg_ulong& timer) {
      if (!timer) timer = cimg::time();
      const cimg_ulong current_time = cimg::time();
      if (current_time>=timer + milliseconds) { timer = current_time; return 0; }
      const unsigned int time_diff = (unsigned int)(timer + milliseconds - current_time);
      timer = current_time + time_diff;
      cimg::sleep(time_diff);
      return time_diff;