String Ogg::XiphComment::comment() const
{
  if(!d->fieldListMap["DESCRIPTION"].isEmpty()) {
    d->commentField = "DESCRIPTION";
    return d->fieldListMap["DESCRIPTION"].front();
  }

  if(!d->fieldListMap["COMMENT"].isEmpty()) {
    d->commentField = "COMMENT";
    return d->fieldListMap["COMMENT"].front();
  }

  return String::null;
}