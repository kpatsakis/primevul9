  void Inspect::operator()(Import_Stub_Ptr import)
  {
    append_indentation();
    append_token("@import", import);
    append_mandatory_space();
    append_string(import->imp_path());
    append_delimiter();
  }