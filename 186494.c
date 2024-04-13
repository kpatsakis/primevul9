  void ADDCALL sass_delete_options (struct Sass_Options* options)
  {
    sass_clear_options(options); free(options);
  }