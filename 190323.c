void t_cpp_generator::generate_exception_what_method(std::ofstream& out, t_struct* tstruct) {
  out << indent();
  generate_exception_what_method_decl(out, tstruct, true);
  out << " {" << endl;

  indent_up();
  out << indent() << "try {" << endl;

  indent_up();
  out << indent() << "std::stringstream ss;" << endl;
  out << indent() << "ss << \"TException - service has thrown: \" << *this;" << endl;
  out << indent() << "this->thriftTExceptionMessageHolder_ = ss.str();" << endl;
  out << indent() << "return this->thriftTExceptionMessageHolder_.c_str();" << endl;
  indent_down();

  out << indent() << "} catch (const std::exception& e) {" << endl;

  indent_up();
  out << indent() << "return \"TException - service has thrown: " << tstruct->get_name() << "\";"
      << endl;
  indent_down();

  out << indent() << "}" << endl;

  indent_down();
  out << "}" << endl << endl;
}