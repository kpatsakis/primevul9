void PortForwardHandler::update(vector<PortForwardDestinationRequest>* requests,
                                vector<PortForwardData>* dataToSend) {
  for (auto& it : sourceHandlers) {
    it->update(dataToSend);
    int fd = it->listen();
    if (fd >= 0) {
      PortForwardDestinationRequest pfr;
      *(pfr.mutable_destination()) = it->getDestination();
      pfr.set_fd(fd);
      requests->push_back(pfr);
    }
  }

  for (auto& it : destinationHandlers) {
    it.second->update(dataToSend);
    if (it.second->getFd() == -1) {
      // Kill the handler and don't update the rest: we'll pick
      // them up later
      destinationHandlers.erase(it.first);
      break;
    }
  }
}