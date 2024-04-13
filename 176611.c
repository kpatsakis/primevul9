  void Inspect::operator()(Null_Ptr n)
  {
    // output the final token
    append_token("null", n);
  }