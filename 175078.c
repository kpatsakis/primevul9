std::string Box_infe::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  sstr << indent << "item_ID: " << m_item_ID << "\n"
       << indent << "item_protection_index: " << m_item_protection_index << "\n"
       << indent << "item_type: " << m_item_type << "\n"
       << indent << "item_name: " << m_item_name << "\n"
       << indent << "content_type: " << m_content_type << "\n"
       << indent << "content_encoding: " << m_content_encoding << "\n"
       << indent << "item uri type: " << m_item_uri_type << "\n"
       << indent << "hidden item: " << std::boolalpha << m_hidden_item << "\n";

  return sstr.str();
}