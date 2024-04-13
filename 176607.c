  void Inspect::operator()(Function_Call_Ptr call)
  {
    append_token(call->name(), call);
    call->arguments()->perform(this);
  }