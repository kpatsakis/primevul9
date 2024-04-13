  void Parser::css_error(const std::string& msg, const std::string& prefix, const std::string& middle, const bool trim)
  {
    int max_len = 18;
    const char* end = this->end;
    while (*end != 0) ++ end;
    const char* pos = peek < optional_spaces >();
    if (!pos) pos = position;

    const char* last_pos(pos);
    if (last_pos > source) {
      utf8::prior(last_pos, source);
    }
    // backup position to last significant char
    while (trim && last_pos > source && last_pos < end) {
      if (!Prelexer::is_space(*last_pos)) break;
      utf8::prior(last_pos, source);
    }

    bool ellipsis_left = false;
    const char* pos_left(last_pos);
    const char* end_left(last_pos);

    if (*pos_left) utf8::next(pos_left, end);
    if (*end_left) utf8::next(end_left, end);
    while (pos_left > source) {
      if (utf8::distance(pos_left, end_left) >= max_len) {
        utf8::prior(pos_left, source);
        ellipsis_left = *(pos_left) != '\n' &&
                        *(pos_left) != '\r';
        utf8::next(pos_left, end);
        break;
      }

      const char* prev = pos_left;
      utf8::prior(prev, source);
      if (*prev == '\r') break;
      if (*prev == '\n') break;
      pos_left = prev;
    }
    if (pos_left < source) {
      pos_left = source;
    }

    bool ellipsis_right = false;
    const char* end_right(pos);
    const char* pos_right(pos);
    while (end_right < end) {
      if (utf8::distance(pos_right, end_right) > max_len) {
        ellipsis_left = *(pos_right) != '\n' &&
                        *(pos_right) != '\r';
        break;
      }
      if (*end_right == '\r') break;
      if (*end_right == '\n') break;
      utf8::next(end_right, end);
    }
    // if (*end_right == 0) end_right ++;

    std::string left(pos_left, end_left);
    std::string right(pos_right, end_right);
    size_t left_subpos = left.size() > 15 ? left.size() - 15 : 0;
    size_t right_subpos = right.size() > 15 ? right.size() - 15 : 0;
    if (left_subpos && ellipsis_left) left = ellipsis + left.substr(left_subpos);
    if (right_subpos && ellipsis_right) right = right.substr(right_subpos) + ellipsis;
    // Hotfix when source is null, probably due to interpolation parsing!?
    if (source == NULL || *source == 0) source = pstate.src;
    // now pass new message to the more generic error function
    error(msg + prefix + quote(left) + middle + quote(right));
  }