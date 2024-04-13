void t_cpp_generator::generate_deserialize_struct(ofstream& out,
                                                  t_struct* tstruct,
                                                  string prefix,
                                                  bool pointer) {
  if (pointer) {
    indent(out) << "if (!" << prefix << ") { " << endl;
    indent(out) << "  " << prefix << " = boost::shared_ptr<" << type_name(tstruct) << ">(new "
                << type_name(tstruct) << ");" << endl;
    indent(out) << "}" << endl;
    indent(out) << "xfer += " << prefix << "->read(iprot);" << endl;
    indent(out) << "bool wasSet = false;" << endl;
    const vector<t_field*>& members = tstruct->get_members();
    vector<t_field*>::const_iterator f_iter;
    for (f_iter = members.begin(); f_iter != members.end(); ++f_iter) {

      indent(out) << "if (" << prefix << "->__isset." << (*f_iter)->get_name()
                  << ") { wasSet = true; }" << endl;
    }
    indent(out) << "if (!wasSet) { " << prefix << ".reset(); }" << endl;
  } else {
    indent(out) << "xfer += " << prefix << ".read(iprot);" << endl;
  }
}