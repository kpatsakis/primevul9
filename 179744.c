ClearServer2Client(rfbClient* client, int messageType)
{
  client->supportedMessages.server2client[((messageType & 0xFF)/8)] &= ~(1<<(messageType % 8));
}