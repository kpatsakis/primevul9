bool Ogg::XiphComment::contains(const String &key) const
{
  return d->fieldListMap.contains(key) && !d->fieldListMap[key].isEmpty();
}