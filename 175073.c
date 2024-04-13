Error Box::write_children(StreamWriter& writer) const
{
  for (const auto& child : m_children) {
    Error err = child->write(writer);
    if (err) {
      return err;
    }
  }

  return Error::Ok;
}