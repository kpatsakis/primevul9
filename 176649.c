  void Inspect::operator()(Parameter_Ptr p)
  {
    append_token(p->name(), p);
    if (p->default_value()) {
      append_colon_separator();
      p->default_value()->perform(this);
    }
    else if (p->is_rest_parameter()) {
      append_string("...");
    }
  }