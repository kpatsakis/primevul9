void generate_field(std::ofstream& out, const t_field* field) {
  generate_field_name(out, field);
  generate_field_value(out, field);
}