void generate_required_field_value(std::ofstream& out, const t_field* field) {
  out << " << to_string(obj." << field->get_name() << ")";
}