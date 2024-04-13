void t_cpp_generator::generate_serialize_field(ofstream& out,
                                               t_field* tfield,
                                               string prefix,
                                               string suffix) {
  t_type* type = get_true_type(tfield->get_type());

  string name = prefix + tfield->get_name() + suffix;

  // Do nothing for void types
  if (type->is_void()) {
    throw "CANNOT GENERATE SERIALIZE CODE FOR void TYPE: " + name;
  }

  if (type->is_struct() || type->is_xception()) {
    generate_serialize_struct(out, (t_struct*)type, name, is_reference(tfield));
  } else if (type->is_container()) {
    generate_serialize_container(out, type, name);
  } else if (type->is_base_type() || type->is_enum()) {

    indent(out) << "xfer += oprot->";

    if (type->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
      switch (tbase) {
      case t_base_type::TYPE_VOID:
        throw "compiler error: cannot serialize void field in a struct: " + name;
        break;
      case t_base_type::TYPE_STRING:
        if (((t_base_type*)type)->is_binary()) {
          out << "writeBinary(" << name << ");";
        } else {
          out << "writeString(" << name << ");";
        }
        break;
      case t_base_type::TYPE_BOOL:
        out << "writeBool(" << name << ");";
        break;
      case t_base_type::TYPE_BYTE:
        out << "writeByte(" << name << ");";
        break;
      case t_base_type::TYPE_I16:
        out << "writeI16(" << name << ");";
        break;
      case t_base_type::TYPE_I32:
        out << "writeI32(" << name << ");";
        break;
      case t_base_type::TYPE_I64:
        out << "writeI64(" << name << ");";
        break;
      case t_base_type::TYPE_DOUBLE:
        out << "writeDouble(" << name << ");";
        break;
      default:
        throw "compiler error: no C++ writer for base type " + t_base_type::t_base_name(tbase)
            + name;
      }
    } else if (type->is_enum()) {
      out << "writeI32((int32_t)" << name << ");";
    }
    out << endl;
  } else {
    printf("DO NOT KNOW HOW TO SERIALIZE FIELD '%s' TYPE '%s'\n",
           name.c_str(),
           type_name(type).c_str());
  }
}