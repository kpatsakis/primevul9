void t_cpp_generator::generate_struct_fingerprint(ofstream& out,
                                                  t_struct* tstruct,
                                                  bool is_definition) {
  string stat, nspace, comment;
  if (is_definition) {
    stat = "";
    nspace = tstruct->get_name() + "::";
    comment = " ";
  } else {
    stat = "static ";
    nspace = "";
    comment = "; // ";
  }

  if (!tstruct->has_fingerprint()) {
    tstruct->generate_fingerprint(); // lazy fingerprint generation
  }
  if (tstruct->has_fingerprint()) {
    out << indent() << stat << "const char* " << nspace << "ascii_fingerprint" << comment << "= \""
        << tstruct->get_ascii_fingerprint() << "\";" << endl << indent() << stat << "const uint8_t "
        << nspace << "binary_fingerprint[" << t_type::fingerprint_len << "]" << comment << "= {";
    const char* comma = "";
    for (int i = 0; i < t_type::fingerprint_len; i++) {
      out << comma << "0x" << t_struct::byte_to_hex(tstruct->get_binary_fingerprint()[i]);
      comma = ",";
    }
    out << "};" << endl << endl;
  }
}