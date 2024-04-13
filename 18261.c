PortForwardDestinationResponse PortForwardHandler::createDestination(
    const PortForwardDestinationRequest& pfdr) {
  int fd = -1;
  bool isTcp = pfdr.destination().has_port();
  if (pfdr.destination().has_port()) {
    // Try ipv6 first
    SocketEndpoint ipv6Localhost;
    ipv6Localhost.set_name("::1");
    ipv6Localhost.set_port(pfdr.destination().port());

    fd = networkSocketHandler->connect(ipv6Localhost);
    if (fd == -1) {
      SocketEndpoint ipv4Localhost;
      ipv4Localhost.set_name("127.0.0.1");
      ipv4Localhost.set_port(pfdr.destination().port());
      // Try ipv4 next
      fd = networkSocketHandler->connect(ipv4Localhost);
    }
  } else {
    fd = pipeSocketHandler->connect(pfdr.destination());
  }
  PortForwardDestinationResponse pfdresponse;
  pfdresponse.set_clientfd(pfdr.fd());
  if (fd == -1) {
    pfdresponse.set_error(strerror(GetErrno()));
  } else {
    int socketId = rand();
    int attempts = 0;
    while (destinationHandlers.find(socketId) != destinationHandlers.end()) {
      socketId = rand();
      attempts++;
      if (attempts >= 100000) {
        pfdresponse.set_error("Could not find empty socket id");
        break;
      }
    }
    if (!pfdresponse.has_error()) {
      LOG(INFO) << "Created socket/fd pair: " << socketId << ' ' << fd;
      destinationHandlers[socketId] =
          shared_ptr<ForwardDestinationHandler>(new ForwardDestinationHandler(
              isTcp ? networkSocketHandler : pipeSocketHandler, fd, socketId));
      pfdresponse.set_socketid(socketId);
    }
  }
  return pfdresponse;
}