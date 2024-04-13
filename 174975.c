std::string Box_imir::dump(Indent& indent) const
{
  std::ostringstream sstr;
  sstr << Box::dump(indent);

  sstr << indent << "mirror axis: ";
  switch (m_axis) {
  case MirrorAxis::Vertical:   sstr << "vertical\n"; break;
  case MirrorAxis::Horizontal: sstr << "horizontal\n"; break;
  }

  return sstr.str();
}