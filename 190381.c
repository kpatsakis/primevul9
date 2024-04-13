void t_cpp_generator::generate_deserialize_field(ofstream& out,
                                                 t_field* tfield,
                                                 string prefix,
                                                 string suffix) {
  t_type* type = get_true_type(tfield->get_type());

  if (type->is_void()) {
    throw "CANNOT GENERATE DESERIALIZE CODE FOR void TYPE: " + prefix + tfield->get_name();
  }

  string name = prefix + tfield->get_name() + suffix;

  if (type->is_struct() || type->is_xception()) {
    generate_deserialize_struct(out, (t_struct*)type, name, is_reference(tfield));
  } else if (type->is_container()) {
    generate_deserialize_container(out, type, name);
  } else if (type->is_base_type()) {
    indent(out) << "xfer += iprot->";
    t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
    switch (tbase) {
    case t_base_type::TYPE_VOID:
      throw "compiler error: cannot serialize void field in a struct: " + name;
      break;
    case t_base_type::TYPE_STRING:
      if (((t_base_type*)type)->is_binary()) {
        out << "readBinary(" << name << ");";
      } else {
        out << "readString(" << name << ");";
      }
      break;
    case t_base_type::TYPE_BOOL:
      out << "readBool(" << name << ");";
      break;
    case t_base_type::TYPE_BYTE:
      out << "readByte(" << name << ");";
      break;
    case t_base_type::TYPE_I16:
      out << "readI16(" << name << ");";
      break;
    case t_base_type::TYPE_I32:
      out << "readI32(" << name << ");";
      break;
    case t_base_type::TYPE_I64:
      out << "readI64(" << name << ");";
      break;
    case t_base_type::TYPE_DOUBLE:
      out << "readDouble(" << name << ");";
      break;
    default:
      throw "compiler error: no C++ reader for base type " + t_base_type::t_base_name(tbase) + name;
    }
    out << endl;
  } else if (type->is_enum()) {
    string t = tmp("ecast");
    out << indent() << "int32_t " << t << ";" << endl << indent() << "xfer += iprot->readI32(" << t
        << ");" << endl << indent() << name << " = (" << type_name(type) << ")" << t << ";" << endl;
  } else {
    printf("DO NOT KNOW HOW TO DESERIALIZE FIELD '%s' TYPE '%s'\n",
           tfield->get_name().c_str(),
           type_name(type).c_str());
  }
}