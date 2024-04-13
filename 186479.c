  static int sass_compile_context (Sass_Context* c_ctx, Context* cpp_ctx)
  {

    // prepare sass compiler with context and options
    Sass_Compiler* compiler = sass_prepare_context(c_ctx, cpp_ctx);

    try {
      // call each compiler step
      sass_compiler_parse(compiler);
      sass_compiler_execute(compiler);
    }
    // pass errors to generic error handler
    catch (...) { handle_errors(c_ctx); }

    sass_delete_compiler(compiler);

    return c_ctx->error_status;
  }