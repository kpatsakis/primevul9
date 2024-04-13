std::string Box_iref::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  for (const auto& ref : m_references) {
    sstr << indent << "reference with type '" << ref.header.get_type_string() << "'"
         << " from ID: " << ref.from_item_ID
         << " to IDs: ";
    for (uint32_t id : ref.to_item_ID) {
      sstr << id << " ";
    }
    sstr << "\n";
  }

  return sstr.str();
}