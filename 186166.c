void Ogg::XiphComment::setComment(const String &s)
{
  addField(d->commentField.isEmpty() ? "DESCRIPTION" : d->commentField, s);
}