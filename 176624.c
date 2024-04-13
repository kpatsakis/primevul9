  void Inspect::operator()(Element_Selector_Ptr s)
  {
    append_token(s->ns_name(), s);
  }