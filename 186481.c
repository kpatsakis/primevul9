  void ADDCALL sass_delete_file_context (struct Sass_File_Context* ctx)
  {
    // clear the context and free it
    sass_clear_context(ctx); free(ctx);
  }