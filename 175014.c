std::string Box_pixi::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  sstr << indent << "bits_per_channel: ";

  for (size_t i=0;i<m_bits_per_channel.size();i++) {
    if (i>0) sstr << ",";
    sstr << ((int)m_bits_per_channel[i]);
  }

  sstr << "\n";

  return sstr.str();
}