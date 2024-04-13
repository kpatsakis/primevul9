  static void sass_reset_options (struct Sass_Options* options)
  {
    // free pointer before
    // or copy/move them
    options->input_path = 0;
    options->output_path = 0;
    options->plugin_path = 0;
    options->include_path = 0;
    options->source_map_file = 0;
    options->source_map_root = 0;
    options->c_functions = 0;
    options->c_importers = 0;
    options->c_headers = 0;
    options->plugin_paths = 0;
    options->include_paths = 0;
  }