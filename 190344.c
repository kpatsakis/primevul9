void generate_optional_field_value(std::ofstream& out, const t_field* field) {
  out << "; (obj.__isset." << field->get_name() << " ? (out";
  generate_required_field_value(out, field);
  out << ") : (out << \"<null>\"))";
}