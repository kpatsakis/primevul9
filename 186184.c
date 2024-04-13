String Ogg::XiphComment::artist() const
{
  if(d->fieldListMap["ARTIST"].isEmpty())
    return String::null;
  return d->fieldListMap["ARTIST"].front();
}