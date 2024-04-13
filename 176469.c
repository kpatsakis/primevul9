  Value* Parser::color_or_string(const std::string& lexed) const
  {
    if (auto color = name_to_color(lexed)) {
      auto c = SASS_MEMORY_NEW(Color_RGBA, color);
      c->is_delayed(true);
      c->pstate(pstate);
      c->disp(lexed);
      return c;
    } else {
      return SASS_MEMORY_NEW(String_Constant, pstate, lexed);
    }
  }