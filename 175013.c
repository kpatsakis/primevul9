std::string Box_irot::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  sstr << indent << "rotation: " << m_rotation << " degrees (CCW)\n";

  return sstr.str();
}