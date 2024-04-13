void generate_field_name(std::ofstream& out, const t_field* field) {
  out << "\"" << field->get_name() << "=\"";
}