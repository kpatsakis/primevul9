string t_cpp_generator::type_name(t_type* ttype, bool in_typedef, bool arg) {
  if (ttype->is_base_type()) {
    string bname = base_type_name(((t_base_type*)ttype)->get_base());
    std::map<string, string>::iterator it = ttype->annotations_.find("cpp.type");
    if (it != ttype->annotations_.end()) {
      bname = it->second;
    }

    if (!arg) {
      return bname;
    }

    if (((t_base_type*)ttype)->get_base() == t_base_type::TYPE_STRING) {
      return "const " + bname + "&";
    } else {
      return "const " + bname;
    }
  }

  // Check for a custom overloaded C++ name
  if (ttype->is_container()) {
    string cname;

    t_container* tcontainer = (t_container*)ttype;
    if (tcontainer->has_cpp_name()) {
      cname = tcontainer->get_cpp_name();
    } else if (ttype->is_map()) {
      t_map* tmap = (t_map*)ttype;
      cname = "std::map<" + type_name(tmap->get_key_type(), in_typedef) + ", "
              + type_name(tmap->get_val_type(), in_typedef) + "> ";
    } else if (ttype->is_set()) {
      t_set* tset = (t_set*)ttype;
      cname = "std::set<" + type_name(tset->get_elem_type(), in_typedef) + "> ";
    } else if (ttype->is_list()) {
      t_list* tlist = (t_list*)ttype;
      cname = "std::vector<" + type_name(tlist->get_elem_type(), in_typedef) + "> ";
    }

    if (arg) {
      return "const " + cname + "&";
    } else {
      return cname;
    }
  }

  string class_prefix;
  if (in_typedef && (ttype->is_struct() || ttype->is_xception())) {
    class_prefix = "class ";
  }

  // Check if it needs to be namespaced
  string pname;
  t_program* program = ttype->get_program();
  if (program != NULL && program != program_) {
    pname = class_prefix + namespace_prefix(program->get_namespace("cpp")) + ttype->get_name();
  } else {
    pname = class_prefix + ttype->get_name();
  }

  if (ttype->is_enum() && !gen_pure_enums_) {
    pname += "::type";
  }

  if (arg) {
    if (is_complex_type(ttype)) {
      return "const " + pname + "&";
    } else {
      return "const " + pname;
    }
  } else {
    return pname;
  }
}