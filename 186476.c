  size_t ADDCALL sass_context_get_included_files_size (struct Sass_Context* ctx)
  { size_t l = 0; auto i = ctx->included_files; while (i && *i) { ++i; ++l; } return l; }