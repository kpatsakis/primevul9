XML_GetCurrentByteIndex(XML_Parser parser)
{
  if (eventPtr)
    return (XML_Index)(parseEndByteIndex - (parseEndPtr - eventPtr));
  return -1;
}