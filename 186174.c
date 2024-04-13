TagLib::uint Ogg::XiphComment::track() const
{
  if(!d->fieldListMap["TRACKNUMBER"].isEmpty())
    return d->fieldListMap["TRACKNUMBER"].front().toInt();
  if(!d->fieldListMap["TRACKNUM"].isEmpty())
    return d->fieldListMap["TRACKNUM"].front().toInt();
  return 0;
}