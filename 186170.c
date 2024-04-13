Ogg::XiphComment::XiphComment(const ByteVector &data) : TagLib::Tag()
{
  d = new XiphCommentPrivate;
  parse(data);
}