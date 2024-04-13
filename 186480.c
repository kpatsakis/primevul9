  static Sass_Compiler* sass_prepare_context (Sass_Context* c_ctx, Context* cpp_ctx) throw()
  {
    try {
      // register our custom functions
      if (c_ctx->c_functions) {
        auto this_func_data = c_ctx->c_functions;
        while (this_func_data && *this_func_data) {
          cpp_ctx->add_c_function(*this_func_data);
          ++this_func_data;
        }
      }

      // register our custom headers
      if (c_ctx->c_headers) {
        auto this_head_data = c_ctx->c_headers;
        while (this_head_data && *this_head_data) {
          cpp_ctx->add_c_header(*this_head_data);
          ++this_head_data;
        }
      }

      // register our custom importers
      if (c_ctx->c_importers) {
        auto this_imp_data = c_ctx->c_importers;
        while (this_imp_data && *this_imp_data) {
          cpp_ctx->add_c_importer(*this_imp_data);
          ++this_imp_data;
        }
      }

      // reset error status
      c_ctx->error_json = 0;
      c_ctx->error_text = 0;
      c_ctx->error_message = 0;
      c_ctx->error_status = 0;
      // reset error position
      c_ctx->error_src = 0;
      c_ctx->error_file = 0;
      c_ctx->error_line = std::string::npos;
      c_ctx->error_column = std::string::npos;

      // allocate a new compiler instance
      void* ctxmem = calloc(1, sizeof(struct Sass_Compiler));
      if (ctxmem == 0) { std::cerr << "Error allocating memory for context" << std::endl; return 0; }
      Sass_Compiler* compiler = (struct Sass_Compiler*) ctxmem;
      compiler->state = SASS_COMPILER_CREATED;

      // store in sass compiler
      compiler->c_ctx = c_ctx;
      compiler->cpp_ctx = cpp_ctx;
      cpp_ctx->c_compiler = compiler;

      // use to parse block
      return compiler;

    }
    // pass errors to generic error handler
    catch (...) { handle_errors(c_ctx); }

    // error
    return 0;

  }