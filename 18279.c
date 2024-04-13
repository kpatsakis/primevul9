PortForwardHandler::PortForwardHandler(
    shared_ptr<SocketHandler> _networkSocketHandler,
    shared_ptr<SocketHandler> _pipeSocketHandler)
    : networkSocketHandler(_networkSocketHandler),
      pipeSocketHandler(_pipeSocketHandler) {}