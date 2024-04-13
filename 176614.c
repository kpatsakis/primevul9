  void Inspect::operator()(Import_Ptr import)
  {
    if (!import->urls().empty()) {
      append_token("@import", import);
      append_mandatory_space();

      import->urls().front()->perform(this);
      if (import->urls().size() == 1) {
        if (import->import_queries()) {
          append_mandatory_space();
          import->import_queries()->perform(this);
        }
      }
      append_delimiter();
      for (size_t i = 1, S = import->urls().size(); i < S; ++i) {
        append_mandatory_linefeed();
        append_token("@import", import);
        append_mandatory_space();

        import->urls()[i]->perform(this);
        if (import->urls().size() - 1 == i) {
          if (import->import_queries()) {
            append_mandatory_space();
            import->import_queries()->perform(this);
          }
        }
        append_delimiter();
      }
    }
  }