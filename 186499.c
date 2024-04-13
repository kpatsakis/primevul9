  size_t ADDCALL sass_option_get_include_path_size(struct Sass_Options* options)
  {
    size_t len = 0;
    struct string_list* cur = options->include_paths;
    while (cur) { len ++; cur = cur->next; }
    return len;
  }