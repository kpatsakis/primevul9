void Ogg::XiphComment::setTrack(uint i)
{
  removeField("TRACKNUM");
  if(i == 0)
    removeField("TRACKNUMBER");
  else
    addField("TRACKNUMBER", String::number(i));
}