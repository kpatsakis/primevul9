  Sass_File_Context* ADDCALL sass_make_file_context(const char* input_path)
  {
    SharedObj::setTaint(true); // needed for static colors
    struct Sass_File_Context* ctx = (struct Sass_File_Context*) calloc(1, sizeof(struct Sass_File_Context));
    if (ctx == 0) { std::cerr << "Error allocating memory for file context" << std::endl; return 0; }
    ctx->type = SASS_CONTEXT_FILE;
    init_options(ctx);
    try {
      if (input_path == 0) { throw(std::runtime_error("File context created without an input path")); }
      if (*input_path == 0) { throw(std::runtime_error("File context created with empty input path")); }
      sass_option_set_input_path(ctx, input_path);
    } catch (...) {
      handle_errors(ctx);
    }
    return ctx;
  }