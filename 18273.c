void PortForwardHandler::closeSourceFd(int fd) {
  for (auto& it : sourceHandlers) {
    if (it->hasUnassignedFd(fd)) {
      it->closeUnassignedFd(fd);
      return;
    }
  }
  STERROR << "Tried to close an unassigned socket that didn't exist (maybe "
             "it was already removed?): "
          << fd;
}