	if(udpLstnSocks != NULL) {
		net.closeUDPListenSockets(udpLstnSocks);
		udpLstnSocks = NULL;
	}