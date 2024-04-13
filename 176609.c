  void Inspect::operator()(Parameters_Ptr p)
  {
    append_string("(");
    if (!p->empty()) {
      (*p)[0]->perform(this);
      for (size_t i = 1, L = p->length(); i < L; ++i) {
        append_comma_separator();
        (*p)[i]->perform(this);
      }
    }
    append_string(")");
  }