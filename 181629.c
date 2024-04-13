SimpleDataBuffer::SimpleDataBuffer(const SimpleDataBuffer & ToClone)
  :DataBuffer((binary *)malloc(ToClone.mySize * sizeof(binary)), ToClone.mySize, myFreeBuffer)
{
  assert(myBuffer != NULL);
  memcpy(myBuffer, ToClone.myBuffer ,mySize );
  bValidValue = ToClone.bValidValue;
}