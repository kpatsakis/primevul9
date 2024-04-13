void PortForwardHandler::closeSourceSocketId(int socketId) {
  auto it = socketIdSourceHandlerMap.find(socketId);
  if (it == socketIdSourceHandlerMap.end()) {
    STERROR << "Tried to close a socket id that doesn't exist";
    return;
  }
  it->second->closeSocket(socketId);
  socketIdSourceHandlerMap.erase(socketId);
}