String Ogg::XiphComment::genre() const
{
  if(d->fieldListMap["GENRE"].isEmpty())
    return String::null;
  return d->fieldListMap["GENRE"].front();
}