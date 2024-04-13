  void Inspect::operator()(Variable_Ptr var)
  {
    append_token(var->name(), var);
  }