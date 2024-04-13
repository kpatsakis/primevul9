  struct Sass_Compiler* ADDCALL sass_make_file_compiler (struct Sass_File_Context* file_ctx)
  {
    if (file_ctx == 0) return 0;
    Context* cpp_ctx = new File_Context(*file_ctx);
    return sass_prepare_context(file_ctx, cpp_ctx);
  }