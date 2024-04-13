void t_cpp_generator::generate_function_helpers(t_service* tservice, t_function* tfunction) {
  if (tfunction->is_oneway()) {
    return;
  }

  std::ofstream& out = (gen_templates_ ? f_service_tcc_ : f_service_);

  t_struct result(program_, tservice->get_name() + "_" + tfunction->get_name() + "_result");
  t_field success(tfunction->get_returntype(), "success", 0);
  if (!tfunction->get_returntype()->is_void()) {
    result.append(&success);
  }

  t_struct* xs = tfunction->get_xceptions();
  const vector<t_field*>& fields = xs->get_members();
  vector<t_field*>::const_iterator f_iter;
  for (f_iter = fields.begin(); f_iter != fields.end(); ++f_iter) {
    result.append(*f_iter);
  }

  generate_struct_declaration(f_header_, &result, false);
  generate_struct_definition(out, f_service_, &result, false);
  generate_struct_reader(out, &result);
  generate_struct_result_writer(out, &result);

  result.set_name(tservice->get_name() + "_" + tfunction->get_name() + "_presult");
  generate_struct_declaration(f_header_, &result, false, true, true, gen_cob_style_);
  generate_struct_definition(out, f_service_, &result, false);
  generate_struct_reader(out, &result, true);
  if (gen_cob_style_) {
    generate_struct_writer(out, &result, true);
  }
}