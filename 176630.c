  void Inspect::operator()(Return_Ptr ret)
  {
    append_indentation();
    append_token("@return", ret);
    append_mandatory_space();
    ret->value()->perform(this);
    append_delimiter();
  }