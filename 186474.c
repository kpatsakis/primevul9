  static int handle_errors(Sass_Context* c_ctx) {
    try { return handle_error(c_ctx); }
    catch (...) { return handle_error(c_ctx); }
  }