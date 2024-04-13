  void run() {
    generate_class_definition();

    // Generate the dispatchCall() function
    generate_dispatch_call(false);
    if (generator_->gen_templates_) {
      generate_dispatch_call(true);
    }

    // Generate all of the process subfunctions
    generate_process_functions();

    generate_factory();
  }