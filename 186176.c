String Ogg::XiphComment::album() const
{
  if(d->fieldListMap["ALBUM"].isEmpty())
    return String::null;
  return d->fieldListMap["ALBUM"].front();
}