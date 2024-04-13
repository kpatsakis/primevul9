  std::string Inspect::rbracket(List_Ptr list) {
    return list->is_bracketed() ? "]" : ")";
  }