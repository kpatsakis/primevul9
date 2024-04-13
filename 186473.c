  Sass_Options* ADDCALL sass_make_options (void)
  {
    struct Sass_Options* options = (struct Sass_Options*) calloc(1, sizeof(struct Sass_Options));
    if (options == 0) { std::cerr << "Error allocating memory for options" << std::endl; return 0; }
    init_options(options);
    return options;
  }