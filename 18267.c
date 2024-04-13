void PortForwardHandler::handlePacket(const Packet& packet,
                                      shared_ptr<Connection> connection) {
  switch (TerminalPacketType(packet.getHeader())) {
    case TerminalPacketType::PORT_FORWARD_DATA: {
      PortForwardData pwd = stringToProto<PortForwardData>(packet.getPayload());
      if (pwd.sourcetodestination()) {
        VLOG(1) << "Got data for destination socket: " << pwd.socketid();
        auto it = destinationHandlers.find(pwd.socketid());
        if (it == destinationHandlers.end()) {
          LOG(WARNING) << "Got data for a socket id that has already closed: "
                       << pwd.socketid();
        } else {
          if (pwd.has_closed()) {
            LOG(INFO) << "Port forward socket closed: " << pwd.socketid();
            it->second->close();
            destinationHandlers.erase(it);
          } else if (pwd.has_error()) {
            // TODO: Probably need to do something better here
            LOG(INFO) << "Port forward socket errored: " << pwd.socketid();
            it->second->close();
            destinationHandlers.erase(it);
          } else {
            it->second->write(pwd.buffer());
          }
        }
      } else {
        if (pwd.has_closed()) {
          LOG(INFO) << "Port forward socket closed: " << pwd.socketid();
          closeSourceSocketId(pwd.socketid());
        } else if (pwd.has_error()) {
          LOG(INFO) << "Port forward socket errored: " << pwd.socketid();
          closeSourceSocketId(pwd.socketid());
        } else {
          VLOG(1) << "Got data for source socket: " << pwd.socketid();
          sendDataToSourceOnSocket(pwd.socketid(), pwd.buffer());
        }
      }
      break;
    }
    case TerminalPacketType::PORT_FORWARD_DESTINATION_REQUEST: {
      PortForwardDestinationRequest pfdr =
          stringToProto<PortForwardDestinationRequest>(packet.getPayload());
      LOG(INFO) << "Got new port destination request for "
                << pfdr.destination();
      PortForwardDestinationResponse pfdresponse = createDestination(pfdr);
      Packet sendPacket(
          uint8_t(TerminalPacketType::PORT_FORWARD_DESTINATION_RESPONSE),
          protoToString(pfdresponse));
      connection->writePacket(sendPacket);
      break;
    }
    case TerminalPacketType::PORT_FORWARD_DESTINATION_RESPONSE: {
      PortForwardDestinationResponse pfdr =
          stringToProto<PortForwardDestinationResponse>(packet.getPayload());
      if (pfdr.has_error()) {
        LOG(INFO) << "Could not connect to server through tunnel: "
                  << pfdr.error();
        closeSourceFd(pfdr.clientfd());
      } else {
        LOG(INFO) << "Received socket/fd map from server: " << pfdr.socketid()
                  << " " << pfdr.clientfd();
        addSourceSocketId(pfdr.socketid(), pfdr.clientfd());
      }
      break;
    }
    default: {
      STFATAL << "Unknown packet type: " << int(packet.getHeader());
    }
  }
}