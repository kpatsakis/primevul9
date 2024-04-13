  void ADDCALL sass_delete_data_context (struct Sass_Data_Context* ctx)
  {
    // clean the source string if it was not passed
    // we reset this member once we start parsing
    if (ctx->source_string) free(ctx->source_string);
    // clear the context and free it
    sass_clear_context(ctx); free(ctx);
  }