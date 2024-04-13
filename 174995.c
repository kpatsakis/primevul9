std::string Box::dump_children(Indent& indent) const
{
  std::ostringstream sstr;

  bool first = true;

  indent++;
  for (const auto& childBox : m_children) {
    if (first) {
      first=false;
    }
    else {
      sstr << indent << "\n";
    }

    sstr << childBox->dump(indent);
  }
  indent--;

  return sstr.str();
}