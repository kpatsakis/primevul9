  void Inspect::operator()(Custom_Warning_Ptr w)
  {
    append_token(w->message(), w);
  }