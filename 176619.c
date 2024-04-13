  void Inspect::operator()(Content_Ptr content)
  {
    append_indentation();
    append_token("@content", content);
    append_delimiter();
  }