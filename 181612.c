DataBuffer * DataBuffer::Clone()
{
  binary *ClonedData = (binary *)malloc(mySize * sizeof(binary));
  assert(ClonedData != NULL);
  memcpy(ClonedData, myBuffer ,mySize );

  SimpleDataBuffer * result = new SimpleDataBuffer(ClonedData, mySize, 0);
  result->bValidValue = bValidValue;
  return result;
}