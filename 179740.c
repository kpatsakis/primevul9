ClearClient2Server(rfbClient* client, int messageType)
{
  client->supportedMessages.client2server[((messageType & 0xFF)/8)] &= ~(1<<(messageType % 8));
}