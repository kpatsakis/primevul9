Error color_profile_raw::write(StreamWriter& writer) const
{
  writer.write(m_data);

    return Error::Ok;
}