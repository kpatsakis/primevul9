void t_cpp_generator::generate_service_processor(t_service* tservice, string style) {
  ProcessorGenerator generator(this, tservice, style);
  generator.run();
}