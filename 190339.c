void t_cpp_generator::generate_struct_definition(ofstream& out,
                                                 ofstream& force_cpp_out,
                                                 t_struct* tstruct,
                                                 bool setters) {
  // Get members
  vector<t_field*>::const_iterator m_iter;
  const vector<t_field*>& members = tstruct->get_members();

  // Destructor
  if (tstruct->annotations_.find("final") == tstruct->annotations_.end()) {
    force_cpp_out << endl << indent() << tstruct->get_name() << "::~" << tstruct->get_name()
                  << "() throw() {" << endl;
    indent_up();

    indent_down();
    force_cpp_out << indent() << "}" << endl << endl;
  }

  // Create a setter function for each field
  if (setters) {
    for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
      if (is_reference((*m_iter))) {
        std::string type = type_name((*m_iter)->get_type());
        out << endl << indent() << "void " << tstruct->get_name() << "::__set_"
            << (*m_iter)->get_name() << "(boost::shared_ptr<"
            << type_name((*m_iter)->get_type(), false, false) << ">";
        out << " val) {" << endl;
      } else {
        out << endl << indent() << "void " << tstruct->get_name() << "::__set_"
            << (*m_iter)->get_name() << "(" << type_name((*m_iter)->get_type(), false, true);
        out << " val) {" << endl;
      }
      indent_up();
      out << indent() << "this->" << (*m_iter)->get_name() << " = val;" << endl;
      indent_down();

      // assume all fields are required except optional fields.
      // for optional fields change __isset.name to true
      bool is_optional = (*m_iter)->get_req() == t_field::T_OPTIONAL;
      if (is_optional) {
        out << indent() << indent() << "__isset." << (*m_iter)->get_name() << " = true;" << endl;
      }
      out << indent() << "}" << endl;
    }
  }
  out << endl;
}