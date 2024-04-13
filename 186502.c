  int ADDCALL sass_compiler_execute(struct Sass_Compiler* compiler)
  {
    if (compiler == 0) return 1;
    if (compiler->state == SASS_COMPILER_EXECUTED) return 0;
    if (compiler->state != SASS_COMPILER_PARSED) return -1;
    if (compiler->c_ctx == NULL) return 1;
    if (compiler->cpp_ctx == NULL) return 1;
    if (compiler->root.isNull()) return 1;
    if (compiler->c_ctx->error_status)
      return compiler->c_ctx->error_status;
    compiler->state = SASS_COMPILER_EXECUTED;
    Context* cpp_ctx = compiler->cpp_ctx;
    Block_Obj root = compiler->root;
    // compile the parsed root block
    try { compiler->c_ctx->output_string = cpp_ctx->render(root); }
    // pass catched errors to generic error handler
    catch (...) { return handle_errors(compiler->c_ctx) | 1; }
    // generate source map json and store on context
    compiler->c_ctx->source_map_string = cpp_ctx->render_srcmap();
    // success
    return 0;
  }