string t_cpp_generator::declare_field(t_field* tfield,
                                      bool init,
                                      bool pointer,
                                      bool constant,
                                      bool reference) {
  // TODO(mcslee): do we ever need to initialize the field?
  string result = "";
  if (constant) {
    result += "const ";
  }
  result += type_name(tfield->get_type());
  if (is_reference(tfield)) {
    result = "boost::shared_ptr<" + result + ">";
  }
  if (pointer) {
    result += "*";
  }
  if (reference) {
    result += "&";
  }
  result += " " + tfield->get_name();
  if (init) {
    t_type* type = get_true_type(tfield->get_type());

    if (type->is_base_type()) {
      t_base_type::t_base tbase = ((t_base_type*)type)->get_base();
      switch (tbase) {
      case t_base_type::TYPE_VOID:
      case t_base_type::TYPE_STRING:
        break;
      case t_base_type::TYPE_BOOL:
        result += " = false";
        break;
      case t_base_type::TYPE_BYTE:
      case t_base_type::TYPE_I16:
      case t_base_type::TYPE_I32:
      case t_base_type::TYPE_I64:
        result += " = 0";
        break;
      case t_base_type::TYPE_DOUBLE:
        result += " = (double)0";
        break;
      default:
        throw "compiler error: no C++ initializer for base type " + t_base_type::t_base_name(tbase);
      }
    } else if (type->is_enum()) {
      result += " = (" + type_name(type) + ")0";
    }
  }
  if (!reference) {
    result += ";";
  }
  return result;
}