void Ogg::XiphComment::addField(const String &key, const String &value, bool replace)
{
  if(replace)
    removeField(key.upper());

  if(!key.isEmpty() && !value.isEmpty())
    d->fieldListMap[key.upper()].append(value);
}