void t_cpp_generator::generate_deserialize_map_element(ofstream& out, t_map* tmap, string prefix) {
  string key = tmp("_key");
  string val = tmp("_val");
  t_field fkey(tmap->get_key_type(), key);
  t_field fval(tmap->get_val_type(), val);

  out << indent() << declare_field(&fkey) << endl;

  generate_deserialize_field(out, &fkey);
  indent(out) << declare_field(&fval, false, false, false, true) << " = " << prefix << "[" << key
              << "];" << endl;

  generate_deserialize_field(out, &fval);
}