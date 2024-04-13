  void Inspect::operator()(Custom_Error_Ptr e)
  {
    append_token(e->message(), e);
  }