    **/
    float frames_per_second() {
      if (!_fps_timer) _fps_timer = cimg::time();
      const float delta = (cimg::time() - _fps_timer)/1000.0f;
      ++_fps_frames;
      if (delta>=1) {
        _fps_fps = _fps_frames/delta;
        _fps_frames = 0;
        _fps_timer = cimg::time();
      }
      return _fps_fps;