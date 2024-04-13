  void Inspect::operator()(Function_Ptr f)
  {
    append_token("get-function", f);
    append_string("(");
    append_string(quote(f->name()));
    append_string(")");
  }