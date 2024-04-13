void PortForwardHandler::addSourceSocketId(int socketId, int sourceFd) {
  for (auto& it : sourceHandlers) {
    if (it->hasUnassignedFd(sourceFd)) {
      it->addSocket(socketId, sourceFd);
      socketIdSourceHandlerMap[socketId] = it;
      return;
    }
  }
  STERROR << "Tried to add a socketId but the corresponding sourceFd is "
             "already dead: "
          << socketId << " " << sourceFd;
}