  void Inspect::operator()(Id_Selector_Ptr s)
  {
    append_token(s->ns_name(), s);
    if (s->has_line_break()) append_optional_linefeed();
    if (s->has_line_break()) append_indentation();
  }