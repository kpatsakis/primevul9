TagLib::uint Ogg::XiphComment::year() const
{
  if(!d->fieldListMap["DATE"].isEmpty())
    return d->fieldListMap["DATE"].front().toInt();
  if(!d->fieldListMap["YEAR"].isEmpty())
    return d->fieldListMap["YEAR"].front().toInt();
  return 0;
}