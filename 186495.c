  static void sass_clear_options (struct Sass_Options* options)
  {
    if (options == 0) return;
    // Deallocate custom functions, headers and importes
    sass_delete_function_list(options->c_functions);
    sass_delete_importer_list(options->c_importers);
    sass_delete_importer_list(options->c_headers);
    // Deallocate inc paths
    if (options->plugin_paths) {
      struct string_list* cur;
      struct string_list* next;
      cur = options->plugin_paths;
      while (cur) {
        next = cur->next;
        free(cur->string);
        free(cur);
        cur = next;
      }
    }
    // Deallocate inc paths
    if (options->include_paths) {
      struct string_list* cur;
      struct string_list* next;
      cur = options->include_paths;
      while (cur) {
        next = cur->next;
        free(cur->string);
        free(cur);
        cur = next;
      }
    }
    // Free options strings
    free(options->input_path);
    free(options->output_path);
    free(options->plugin_path);
    free(options->include_path);
    free(options->source_map_file);
    free(options->source_map_root);
    // Reset our pointers
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