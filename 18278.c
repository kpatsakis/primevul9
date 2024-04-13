TerminalClient::TerminalClient(shared_ptr<SocketHandler> _socketHandler,
                               shared_ptr<SocketHandler> _pipeSocketHandler,
                               const SocketEndpoint& _socketEndpoint,
                               const string& id, const string& passkey,
                               shared_ptr<Console> _console, bool jumphost,
                               const string& tunnels,
                               const string& reverseTunnels,
                               bool forwardSshAgent,
                               const string& identityAgent)
    : console(_console), shuttingDown(false) {
  portForwardHandler = shared_ptr<PortForwardHandler>(
      new PortForwardHandler(_socketHandler, _pipeSocketHandler));
  InitialPayload payload;
  payload.set_jumphost(jumphost);

  try {
    if (tunnels.length()) {
      auto pfsrs = parseRangesToRequests(tunnels);
      for (auto& pfsr : pfsrs) {
#ifdef WIN32
        STFATAL << "Source tunnel not supported on windows yet";
#else
        auto pfsresponse =
            portForwardHandler->createSource(pfsr, nullptr, -1, -1);
        if (pfsresponse.has_error()) {
          throw std::runtime_error(pfsresponse.error());
        }
#endif
      }
    }
    if (reverseTunnels.length()) {
      auto pfsrs = parseRangesToRequests(reverseTunnels);
      for (auto& pfsr : pfsrs) {
        *(payload.add_reversetunnels()) = pfsr;
      }
    }
    if (forwardSshAgent) {
      PortForwardSourceRequest pfsr;
      string authSock = "";
      if (identityAgent.length()) {
        authSock.assign(identityAgent);
      } else {
        auto authSockEnv = getenv("SSH_AUTH_SOCK");
        if (!authSockEnv) {
          CLOG(INFO, "stdout")
              << "Missing environment variable SSH_AUTH_SOCK.  Are you sure "
                 "you "
                 "ran ssh-agent first?"
              << endl;
          exit(1);
        }
        authSock.assign(authSockEnv);
      }
      if (authSock.length()) {
        pfsr.mutable_destination()->set_name(authSock);
        pfsr.set_environmentvariable("SSH_AUTH_SOCK");
        *(payload.add_reversetunnels()) = pfsr;
      }
    }
  } catch (const std::runtime_error& ex) {
    CLOG(INFO, "stdout") << "Error establishing port forward: " << ex.what()
                         << endl;
    exit(1);
  }

  connection = shared_ptr<ClientConnection>(
      new ClientConnection(_socketHandler, _socketEndpoint, id, passkey));

  int connectFailCount = 0;
  while (true) {
    try {
      bool fail = true;
      if (connection->connect()) {
        connection->writePacket(
            Packet(EtPacketType::INITIAL_PAYLOAD, protoToString(payload)));
        fd_set rfd;
        timeval tv;
        for (int a = 0; a < 3; a++) {
          FD_ZERO(&rfd);
          int clientFd = connection->getSocketFd();
          if (clientFd < 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
          }
          FD_SET(clientFd, &rfd);
          tv.tv_sec = 1;
          tv.tv_usec = 0;
          select(clientFd + 1, &rfd, NULL, NULL, &tv);
          if (FD_ISSET(clientFd, &rfd)) {
            Packet initialResponsePacket;
            if (connection->readPacket(&initialResponsePacket)) {
              if (initialResponsePacket.getHeader() !=
                  EtPacketType::INITIAL_RESPONSE) {
                CLOG(INFO, "stdout") << "Error: Missing initial response\n";
                STFATAL << "Missing initial response!";
              }
              auto initialResponse = stringToProto<InitialResponse>(
                  initialResponsePacket.getPayload());
              if (initialResponse.has_error()) {
                CLOG(INFO, "stdout") << "Error initializing connection: "
                                     << initialResponse.error() << endl;
                exit(1);
              }
              fail = false;
              break;
            }
          }
        }
      }
      if (fail) {
        LOG(WARNING) << "Connecting to server failed: Connect timeout";
        connectFailCount++;
        if (connectFailCount == 3) {
          throw std::runtime_error("Connect Timeout");
        }
      }
    } catch (const runtime_error& err) {
      LOG(INFO) << "Could not make initial connection to server";
      CLOG(INFO, "stdout") << "Could not make initial connection to "
                           << _socketEndpoint << ": " << err.what() << endl;
      exit(1);
    }

    TelemetryService::get()->logToDatadog("Connection Established",
                                          el::Level::Info, __FILE__, __LINE__);
    break;
  }
  VLOG(1) << "Client created with id: " << connection->getId();
};