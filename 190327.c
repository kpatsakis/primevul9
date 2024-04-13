void t_cpp_generator::generate_enum(t_enum* tenum) {
  vector<t_enum_value*> constants = tenum->get_constants();

  std::string enum_name = tenum->get_name();
  if (!gen_pure_enums_) {
    enum_name = "type";
    f_types_ << indent() << "struct " << tenum->get_name() << " {" << endl;
    indent_up();
  }
  f_types_ << indent() << "enum " << enum_name;

  generate_enum_constant_list(f_types_, constants, "", "", true);

  if (!gen_pure_enums_) {
    indent_down();
    f_types_ << "};" << endl;
  }

  f_types_ << endl;

  /**
     Generate a character array of enum names for debugging purposes.
  */
  std::string prefix = "";
  if (!gen_pure_enums_) {
    prefix = tenum->get_name() + "::";
  }

  f_types_impl_ << indent() << "int _k" << tenum->get_name() << "Values[] =";
  generate_enum_constant_list(f_types_impl_, constants, prefix.c_str(), "", false);

  f_types_impl_ << indent() << "const char* _k" << tenum->get_name() << "Names[] =";
  generate_enum_constant_list(f_types_impl_, constants, "\"", "\"", false);

  f_types_ << indent() << "extern const std::map<int, const char*> _" << tenum->get_name()
           << "_VALUES_TO_NAMES;" << endl << endl;

  f_types_impl_ << indent() << "const std::map<int, const char*> _" << tenum->get_name()
                << "_VALUES_TO_NAMES(::apache::thrift::TEnumIterator(" << constants.size() << ", _k"
                << tenum->get_name() << "Values"
                << ", _k" << tenum->get_name() << "Names), "
                << "::apache::thrift::TEnumIterator(-1, NULL, NULL));" << endl << endl;

  generate_local_reflection(f_types_, tenum, false);
  generate_local_reflection(f_types_impl_, tenum, true);
}