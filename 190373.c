string t_cpp_generator::base_type_name(t_base_type::t_base tbase) {
  switch (tbase) {
  case t_base_type::TYPE_VOID:
    return "void";
  case t_base_type::TYPE_STRING:
    return "std::string";
  case t_base_type::TYPE_BOOL:
    return "bool";
  case t_base_type::TYPE_BYTE:
    return "int8_t";
  case t_base_type::TYPE_I16:
    return "int16_t";
  case t_base_type::TYPE_I32:
    return "int32_t";
  case t_base_type::TYPE_I64:
    return "int64_t";
  case t_base_type::TYPE_DOUBLE:
    return "double";
  default:
    throw "compiler error: no C++ base type name for base type " + t_base_type::t_base_name(tbase);
  }
}