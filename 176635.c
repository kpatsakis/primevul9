  void Inspect::operator()(Compound_Selector_Ptr s)
  {
    for (size_t i = 0, L = s->length(); i < L; ++i) {
      (*s)[i]->perform(this);
    }
    if (s->has_line_break()) {
      if (output_style() != COMPACT) {
        append_optional_linefeed();
      }
    }
  }