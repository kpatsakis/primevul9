void t_cpp_generator::generate_serialize_struct(ofstream& out,
                                                t_struct* tstruct,
                                                string prefix,
                                                bool pointer) {
  if (pointer) {
    indent(out) << "if (" << prefix << ") {" << endl;
    indent(out) << "  xfer += " << prefix << "->write(oprot); " << endl;
    indent(out) << "} else {"
                << "oprot->writeStructBegin(\"" << tstruct->get_name() << "\"); " << endl;
    indent(out) << "  oprot->writeStructEnd();" << endl;
    indent(out) << "  oprot->writeFieldStop();" << endl;
    indent(out) << "}" << endl;
  } else {
    indent(out) << "xfer += " << prefix << ".write(oprot);" << endl;
  }
}