  void Inspect::operator()(Placeholder_Selector_Ptr s)
  {
    append_token(s->name(), s);
    if (s->has_line_break()) append_optional_linefeed();
    if (s->has_line_break()) append_indentation();

  }