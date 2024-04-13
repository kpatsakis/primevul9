string t_cpp_generator::function_signature(t_function* tfunction,
                                           string style,
                                           string prefix,
                                           bool name_params) {
  t_type* ttype = tfunction->get_returntype();
  t_struct* arglist = tfunction->get_arglist();
  bool has_xceptions = !tfunction->get_xceptions()->get_members().empty();

  if (style == "") {
    if (is_complex_type(ttype)) {
      return "void " + prefix + tfunction->get_name() + "(" + type_name(ttype)
             + (name_params ? "& _return" : "& /* _return */")
             + argument_list(arglist, name_params, true) + ")";
    } else {
      return type_name(ttype) + " " + prefix + tfunction->get_name() + "("
             + argument_list(arglist, name_params) + ")";
    }
  } else if (style.substr(0, 3) == "Cob") {
    string cob_type;
    string exn_cob;
    if (style == "CobCl") {
      cob_type = "(" + service_name_ + "CobClient";
      if (gen_templates_) {
        cob_type += "T<Protocol_>";
      }
      cob_type += "* client)";
    } else if (style == "CobSv") {
      cob_type = (ttype->is_void() ? "()" : ("(" + type_name(ttype) + " const& _return)"));
      if (has_xceptions) {
        exn_cob
            = ", tcxx::function<void(::apache::thrift::TDelayedException* _throw)> /* exn_cob */";
      }
    } else {
      throw "UNKNOWN STYLE";
    }

    return "void " + prefix + tfunction->get_name() + "(tcxx::function<void" + cob_type + "> cob"
           + exn_cob + argument_list(arglist, name_params, true) + ")";
  } else {
    throw "UNKNOWN STYLE";
  }
}