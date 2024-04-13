void t_cpp_generator::generate_service_helpers(t_service* tservice) {
  vector<t_function*> functions = tservice->get_functions();
  vector<t_function*>::iterator f_iter;
  std::ofstream& out = (gen_templates_ ? f_service_tcc_ : f_service_);

  for (f_iter = functions.begin(); f_iter != functions.end(); ++f_iter) {
    t_struct* ts = (*f_iter)->get_arglist();
    string name_orig = ts->get_name();

    // TODO(dreiss): Why is this stuff not in generate_function_helpers?
    ts->set_name(tservice->get_name() + "_" + (*f_iter)->get_name() + "_args");
    generate_struct_declaration(f_header_, ts, false);
    generate_struct_definition(out, f_service_, ts, false);
    generate_struct_reader(out, ts);
    generate_struct_writer(out, ts);
    ts->set_name(tservice->get_name() + "_" + (*f_iter)->get_name() + "_pargs");
    generate_struct_declaration(f_header_, ts, false, true, false, true);
    generate_struct_definition(out, f_service_, ts, false);
    generate_struct_writer(out, ts, true);
    ts->set_name(name_orig);

    generate_function_helpers(tservice, *f_iter);
  }
}