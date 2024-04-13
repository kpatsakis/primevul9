  int ADDCALL sass_compile_file_context(Sass_File_Context* file_ctx)
  {
    if (file_ctx == 0) return 1;
    if (file_ctx->error_status)
      return file_ctx->error_status;
    try {
      if (file_ctx->input_path == 0) { throw(std::runtime_error("File context has no input path")); }
      if (*file_ctx->input_path == 0) { throw(std::runtime_error("File context has empty input path")); }
    }
    catch (...) { return handle_errors(file_ctx) | 1; }
    Context* cpp_ctx = new File_Context(*file_ctx);
    return sass_compile_context(file_ctx, cpp_ctx);
  }