void ProcessorGenerator::generate_process_functions() {
  vector<t_function*> functions = service_->get_functions();
  vector<t_function*>::iterator f_iter;
  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    if (generator_->gen_templates_) {
      generator_->generate_process_function(service_, *f_iter, style_, false);
      generator_->generate_process_function(service_, *f_iter, style_, true);
    } else {
      generator_->generate_process_function(service_, *f_iter, style_, false);
    }
  }
}