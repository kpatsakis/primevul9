void t_cpp_generator::generate_local_reflection(std::ofstream& out,
                                                t_type* ttype,
                                                bool is_definition) {
  if (!gen_dense_) {
    return;
  }
  ttype = get_true_type(ttype);
  string key = ttype->get_ascii_fingerprint() + (is_definition ? "-defn" : "-decl");
  assert(ttype->has_fingerprint()); // test AFTER get due to lazy fingerprint generation

  // Note that we have generated this fingerprint.  If we already did, bail out.
  if (!reflected_fingerprints_.insert(key).second) {
    return;
  }
  // Let each program handle its own structures.
  if (ttype->get_program() != NULL && ttype->get_program() != program_) {
    return;
  }

  // Do dependencies.
  if (ttype->is_list()) {
    generate_local_reflection(out, ((t_list*)ttype)->get_elem_type(), is_definition);
  } else if (ttype->is_set()) {
    generate_local_reflection(out, ((t_set*)ttype)->get_elem_type(), is_definition);
  } else if (ttype->is_map()) {
    generate_local_reflection(out, ((t_map*)ttype)->get_key_type(), is_definition);
    generate_local_reflection(out, ((t_map*)ttype)->get_val_type(), is_definition);
  } else if (ttype->is_struct() || ttype->is_xception()) {
    // Hacky hacky.  For efficiency and convenience, we need a dummy "T_STOP"
    // type at the end of our typespec array.  Unfortunately, there is no
    // T_STOP type, so we use the global void type, and special case it when
    // generating its typespec.

    const vector<t_field*>& members = ((t_struct*)ttype)->get_sorted_members();
    vector<t_field*>::const_iterator m_iter;
    for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
      generate_local_reflection(out, (**m_iter).get_type(), is_definition);
    }
    generate_local_reflection(out, g_type_void, is_definition);

    // For definitions of structures, do the arrays of metas and field specs also.
    if (is_definition) {
      out << indent() << "::apache::thrift::reflection::local::FieldMeta" << endl << indent()
          << local_reflection_name("metas", ttype) << "[] = {" << endl;
      indent_up();
      for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
        indent(out) << "{ " << (*m_iter)->get_key() << ", "
                    << (((*m_iter)->get_req() == t_field::T_OPTIONAL) ? "true" : "false") << " },"
                    << endl;
      }
      // Zero for the T_STOP marker.
      indent(out) << "{ 0, false }" << endl << "};" << endl;
      indent_down();

      out << indent() << "::apache::thrift::reflection::local::TypeSpec*" << endl << indent()
          << local_reflection_name("specs", ttype) << "[] = {" << endl;
      indent_up();
      for (m_iter = members.begin(); m_iter != members.end(); ++m_iter) {
        indent(out) << "&" << local_reflection_name("typespec", (*m_iter)->get_type(), true) << ","
                    << endl;
      }
      indent(out) << "&" << local_reflection_name("typespec", g_type_void) << "," << endl;
      indent_down();
      indent(out) << "};" << endl;
    }
  }

  out << indent() << "// " << ttype->get_fingerprint_material() << endl << indent()
      << (is_definition ? "" : "extern ") << "::apache::thrift::reflection::local::TypeSpec" << endl
      << local_reflection_name("typespec", ttype) << (is_definition ? "(" : ";") << endl;

  if (!is_definition) {
    out << endl;
    return;
  }

  indent_up();

  if (ttype->is_void()) {
    indent(out) << "::apache::thrift::protocol::T_STOP";
  } else {
    indent(out) << type_to_enum(ttype);
  }

  if (ttype->is_struct()) {
    out << "," << endl << indent() << type_name(ttype) << "::binary_fingerprint," << endl
        << indent() << local_reflection_name("metas", ttype) << "," << endl << indent()
        << local_reflection_name("specs", ttype);
  } else if (ttype->is_list()) {
    out << "," << endl << indent() << "&"
        << local_reflection_name("typespec", ((t_list*)ttype)->get_elem_type(), true) << "," << endl
        << indent() << "NULL";
  } else if (ttype->is_set()) {
    out << "," << endl << indent() << "&"
        << local_reflection_name("typespec", ((t_set*)ttype)->get_elem_type(), true) << "," << endl
        << indent() << "NULL";
  } else if (ttype->is_map()) {
    out << "," << endl << indent() << "&"
        << local_reflection_name("typespec", ((t_map*)ttype)->get_key_type(), true) << "," << endl
        << indent() << "&"
        << local_reflection_name("typespec", ((t_map*)ttype)->get_val_type(), true);
  }

  out << ");" << endl << endl;

  indent_down();
}