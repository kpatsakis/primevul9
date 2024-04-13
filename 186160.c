void Ogg::XiphComment::removeField(const String &key, const String &value)
{
  if(!value.isNull()) {
    StringList::Iterator it = d->fieldListMap[key].begin();
    while(it != d->fieldListMap[key].end()) {
      if(value == *it)
        it = d->fieldListMap[key].erase(it);
      else
        it++;
    }
  }
  else
    d->fieldListMap.erase(key);
}