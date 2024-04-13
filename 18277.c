void TerminalClient::run(const string& command) {
  if (console) {
    console->setup();
  }

// TE sends/receives data to/from the shell one char at a time.
#define BUF_SIZE (16 * 1024)
  char b[BUF_SIZE];

  time_t keepaliveTime = time(NULL) + CLIENT_KEEP_ALIVE_DURATION;
  bool waitingOnKeepalive = false;

  if (command.length()) {
    LOG(INFO) << "Got command: " << command;
    et::TerminalBuffer tb;
    tb.set_buffer(command + "; exit\n");

    connection->writePacket(
        Packet(TerminalPacketType::TERMINAL_BUFFER, protoToString(tb)));
  }

  TerminalInfo lastTerminalInfo;

  if (!console.get()) {
    CLOG(INFO, "stdout") << "ET running, feel free to background..." << endl;
  }

  while (!connection->isShuttingDown()) {
    {
      lock_guard<recursive_mutex> guard(shutdownMutex);
      if (shuttingDown) {
        break;
      }
    }
    // Data structures needed for select() and
    // non-blocking I/O.
    fd_set rfd;
    timeval tv;

    FD_ZERO(&rfd);
    int maxfd = -1;
    int consoleFd = -1;
    if (console) {
      consoleFd = console->getFd();
      maxfd = consoleFd;
      FD_SET(consoleFd, &rfd);
    }
    int clientFd = connection->getSocketFd();
    if (clientFd > 0) {
      FD_SET(clientFd, &rfd);
      maxfd = max(maxfd, clientFd);
    }
    // TODO: set port forward sockets as well for performance reasons.
    tv.tv_sec = 0;
    tv.tv_usec = 10000;
    select(maxfd + 1, &rfd, NULL, NULL, &tv);

    try {
      if (console) {
        // Check for data to send.
        if (FD_ISSET(consoleFd, &rfd)) {
          // Read from stdin and write to our client that will then send it to
          // the server.
          VLOG(4) << "Got data from stdin";
#ifdef WIN32
          DWORD events;
          INPUT_RECORD buffer[128];
          HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
          PeekConsoleInput(handle, buffer, 128, &events);
          if (events > 0) {
            ReadConsoleInput(handle, buffer, 128, &events);
            string s;
            for (int keyEvent = 0; keyEvent < events; keyEvent++) {
              if (buffer[keyEvent].EventType == KEY_EVENT &&
                  buffer[keyEvent].Event.KeyEvent.bKeyDown) {
                char charPressed =
                    ((char)buffer[keyEvent].Event.KeyEvent.uChar.AsciiChar);
                if (charPressed) {
                  s += charPressed;
                }
              }
            }
            if (s.length()) {
              et::TerminalBuffer tb;
              tb.set_buffer(s);

              connection->writePacket(Packet(
                  TerminalPacketType::TERMINAL_BUFFER, protoToString(tb)));
              keepaliveTime = time(NULL) + CLIENT_KEEP_ALIVE_DURATION;
            }
          }
#else
          if (console) {
            int rc = ::read(consoleFd, b, BUF_SIZE);
            FATAL_FAIL(rc);
            if (rc > 0) {
              // VLOG(1) << "Sending byte: " << int(b) << " " << char(b) << " "
              // << connection->getWriter()->getSequenceNumber();
              string s(b, rc);
              et::TerminalBuffer tb;
              tb.set_buffer(s);

              connection->writePacket(Packet(
                  TerminalPacketType::TERMINAL_BUFFER, protoToString(tb)));
              keepaliveTime = time(NULL) + CLIENT_KEEP_ALIVE_DURATION;
            }
          }
#endif
        }
      }

      if (clientFd > 0 && FD_ISSET(clientFd, &rfd)) {
        VLOG(4) << "Clientfd is selected";
        while (connection->hasData()) {
          VLOG(4) << "connection has data";
          Packet packet;
          if (!connection->read(&packet)) {
            break;
          }
          uint8_t packetType = packet.getHeader();
          if (packetType == et::TerminalPacketType::PORT_FORWARD_DATA ||
              packetType ==
                  et::TerminalPacketType::PORT_FORWARD_DESTINATION_REQUEST ||
              packetType ==
                  et::TerminalPacketType::PORT_FORWARD_DESTINATION_RESPONSE) {
            keepaliveTime = time(NULL) + CLIENT_KEEP_ALIVE_DURATION;
            VLOG(4) << "Got PF packet type " << packetType;
            portForwardHandler->handlePacket(packet, connection);
            continue;
          }
          switch (packetType) {
            case et::TerminalPacketType::TERMINAL_BUFFER: {
              if (console) {
                VLOG(3) << "Got terminal buffer";
                // Read from the server and write to our fake terminal
                et::TerminalBuffer tb =
                    stringToProto<et::TerminalBuffer>(packet.getPayload());
                const string& s = tb.buffer();
                // VLOG(5) << "Got message: " << s;
                // VLOG(1) << "Got byte: " << int(b) << " " << char(b) << " " <<
                // connection->getReader()->getSequenceNumber();
                keepaliveTime = time(NULL) + CLIENT_KEEP_ALIVE_DURATION;
                console->write(s);
              }
              break;
            }
            case et::TerminalPacketType::KEEP_ALIVE:
              waitingOnKeepalive = false;
              // This will fill up log file quickly but is helpful for debugging
              // latency issues.
              LOG(INFO) << "Got a keepalive";
              break;
            default:
              STFATAL << "Unknown packet type: " << int(packetType);
          }
        }
      }

      if (clientFd > 0 && keepaliveTime < time(NULL)) {
        keepaliveTime = time(NULL) + CLIENT_KEEP_ALIVE_DURATION;
        if (waitingOnKeepalive) {
          LOG(INFO) << "Missed a keepalive, killing connection.";
          connection->closeSocketAndMaybeReconnect();
          waitingOnKeepalive = false;
        } else {
          LOG(INFO) << "Writing keepalive packet";
          connection->writePacket(Packet(TerminalPacketType::KEEP_ALIVE, ""));
          waitingOnKeepalive = true;
        }
      }
      if (clientFd < 0) {
        // We are disconnected, so stop waiting for keepalive.
        waitingOnKeepalive = false;
      }

      if (console) {
        TerminalInfo ti = console->getTerminalInfo();

        if (ti != lastTerminalInfo) {
          LOG(INFO) << "Window size changed: row: " << ti.row()
                    << " column: " << ti.column() << " width: " << ti.width()
                    << " height: " << ti.height();
          lastTerminalInfo = ti;
          connection->writePacket(
              Packet(TerminalPacketType::TERMINAL_INFO, protoToString(ti)));
        }
      }

      vector<PortForwardDestinationRequest> requests;
      vector<PortForwardData> dataToSend;
      portForwardHandler->update(&requests, &dataToSend);
      for (auto& pfr : requests) {
        connection->writePacket(
            Packet(TerminalPacketType::PORT_FORWARD_DESTINATION_REQUEST,
                   protoToString(pfr)));
        VLOG(4) << "send PF request";
        keepaliveTime = time(NULL) + CLIENT_KEEP_ALIVE_DURATION;
      }
      for (auto& pwd : dataToSend) {
        connection->writePacket(
            Packet(TerminalPacketType::PORT_FORWARD_DATA, protoToString(pwd)));
        VLOG(4) << "send PF data";
        keepaliveTime = time(NULL) + CLIENT_KEEP_ALIVE_DURATION;
      }
    } catch (const runtime_error& re) {
      STERROR << "Error: " << re.what();
      CLOG(INFO, "stdout") << "Connection closing because of error: "
                           << re.what() << endl;
      lock_guard<recursive_mutex> guard(shutdownMutex);
      shuttingDown = true;
    }
  }
  if (console) {
    console->teardown();
  }
  CLOG(INFO, "stdout") << "Session terminated" << endl;
}