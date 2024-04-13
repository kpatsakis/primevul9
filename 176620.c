  void Inspect::operator()(String_Schema_Ptr ss)
  {
    // Evaluation should turn these into String_Constants,
    // so this method is only for inspection purposes.
    for (size_t i = 0, L = ss->length(); i < L; ++i) {
      if ((*ss)[i]->is_interpolant()) append_string("#{");
      (*ss)[i]->perform(this);
      if ((*ss)[i]->is_interpolant()) append_string("}");
    }
  }