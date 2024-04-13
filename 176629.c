  std::string Inspect::lbracket(List_Ptr list) {
    return list->is_bracketed() ? "[" : "(";
  }