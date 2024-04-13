String Ogg::XiphComment::title() const
{
  if(d->fieldListMap["TITLE"].isEmpty())
    return String::null;
  return d->fieldListMap["TITLE"].front();
}