void generate_field_value(std::ofstream& out, const t_field* field) {
  if (field->get_req() == t_field::T_OPTIONAL)
    generate_optional_field_value(out, field);
  else
    generate_required_field_value(out, field);
}