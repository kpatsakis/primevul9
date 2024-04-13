  static Block_Obj sass_parse_block(Sass_Compiler* compiler) throw()
  {

    // assert valid pointer
    if (compiler == 0) return {};
    // The cpp context must be set by now
    Context* cpp_ctx = compiler->cpp_ctx;
    Sass_Context* c_ctx = compiler->c_ctx;
    // We will take care to wire up the rest
    compiler->cpp_ctx->c_compiler = compiler;
    compiler->state = SASS_COMPILER_PARSED;

    try {

      // get input/output path from options
      std::string input_path = safe_str(c_ctx->input_path);
      std::string output_path = safe_str(c_ctx->output_path);

      // maybe skip some entries of included files
      // we do not include stdin for data contexts
      bool skip = c_ctx->type == SASS_CONTEXT_DATA;

      // dispatch parse call
      Block_Obj root(cpp_ctx->parse());
      // abort on errors
      if (!root) return {};

      // skip all prefixed files? (ToDo: check srcmap)
      // IMO source-maps should point to headers already
      // therefore don't skip it for now. re-enable or
      // remove completely once this is tested
      size_t headers = cpp_ctx->head_imports;

      // copy the included files on to the context (dont forget to free later)
      if (copy_strings(cpp_ctx->get_included_files(skip, headers), &c_ctx->included_files) == NULL)
        throw(std::bad_alloc());

      // return parsed block
      return root;

    }
    // pass errors to generic error handler
    catch (...) { handle_errors(c_ctx); }

    // error
    return {};

  }