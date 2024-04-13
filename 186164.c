TagLib::uint Ogg::XiphComment::fieldCount() const
{
  uint count = 0;

  FieldListMap::ConstIterator it = d->fieldListMap.begin();
  for(; it != d->fieldListMap.end(); ++it)
    count += (*it).second.size();

  return count;
}