  const char* ADDCALL sass_option_get_include_path(struct Sass_Options* options, size_t i)
  {
    struct string_list* cur = options->include_paths;
    while (i) { i--; cur = cur->next; }
    return cur->string;
  }