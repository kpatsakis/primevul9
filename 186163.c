void Ogg::XiphComment::setYear(uint i)
{
  removeField("YEAR");
  if(i == 0)
    removeField("DATE");
  else
    addField("DATE", String::number(i));
}