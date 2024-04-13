void PortForwardHandler::sendDataToSourceOnSocket(int socketId,
                                                  const string& data) {
  auto it = socketIdSourceHandlerMap.find(socketId);
  if (it == socketIdSourceHandlerMap.end()) {
    STERROR << "Tried to send data on a socket id that doesn't exist: "
            << socketId;
    return;
  }
  it->second->sendDataOnSocket(socketId, data);
}