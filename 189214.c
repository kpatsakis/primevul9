ZrtpQueue::rtpDataPacket(unsigned char* buffer, int32 rtn, InetHostAddress network_address, tpport_t transport_port)
{
     // Special handling of padding to take care of encrypted content.
    // In case of SRTP the padding length field is also encrypted, thus
    // it gives a wrong length. Check and clear padding bit before
    // creating the RTPPacket. Will be set and re-computed after a possible
    // SRTP decryption.
    uint8 padSet = (*buffer & 0x20);
    if (padSet) {
        *buffer = *buffer & ~0x20;          // clear padding bit
    }
    //  build a packet. It will link itself to its source
    IncomingRTPPkt* packet =
        new IncomingRTPPkt(buffer,rtn);

    // Generic header validity check.
    if ( !packet->isHeaderValid() ) {
        delete packet;
        return 0;
    }

    // Look for a CryptoContext for this packet's SSRC
    CryptoContext* pcc = getInQueueCryptoContext(packet->getSSRC());

    // If no crypto context is available for this SSRC but we are already in
    // Secure state then create a CryptoContext for this SSRC.
    // Assumption: every SSRC stream sent via this connection is secured
    // _and_ uses the same crypto parameters.
    if (pcc == NULL) {
        pcc = getInQueueCryptoContext(0);
        if (pcc != NULL) {
            pcc = pcc->newCryptoContextForSSRC(packet->getSSRC(), 0, 0L);
            if (pcc != NULL) {
                pcc->deriveSrtpKeys(0);
                setInQueueCryptoContext(pcc);
            }
        }
    }
    // If no crypto context: then either ZRTP is off or in early state
    // If crypto context is available then unprotect data here. If an error
    // occurs report the error and discard the packet.
    if (pcc != NULL) {
        int32 ret;
        if ((ret = packet->unprotect(pcc)) < 0) {
            if (!onSRTPPacketError(*packet, ret)) {
                delete packet;
                return 0;
            }
        }
        if (started && zrtpEngine->inState(WaitConfAck)) {
            zrtpEngine->conf2AckSecure();
        }
    }

    // virtual for profile-specific validation and processing.
    if (!onRTPPacketRecv(*packet) ) {
        delete packet;
        return 0;
    }
    if (padSet) {
        packet->reComputePayLength(true);
    }
    // get time of arrival
    struct timeval recvtime;
    gettimeofday(&recvtime,NULL);

    bool source_created;
    SyncSourceLink* sourceLink =
            getSourceBySSRC(packet->getSSRC(),source_created);
    SyncSource* s = sourceLink->getSource();
    if ( source_created ) {
        // Set data transport address.
        setDataTransportPort(*s,transport_port);
        // Network address is assumed to be the same as the control one
        setNetworkAddress(*s,network_address);
        sourceLink->initStats();
        // First packet arrival time.
        sourceLink->setInitialDataTime(recvtime);
        sourceLink->setProbation(getMinValidPacketSequence());
        if ( sourceLink->getHello() )
            onNewSyncSource(*s);
    }
    else if ( 0 == s->getDataTransportPort() ) {
        // Test if RTCP packets had been received but this is the
        // first data packet from this source.
        setDataTransportPort(*s,transport_port);
    }

    // Before inserting in the queue,
    // 1) check for collisions and loops. If the packet cannot be
    //    assigned to a source, it will be rejected.
    // 2) check the source is a sufficiently well known source
    // TODO: also check CSRC identifiers.
    if (checkSSRCInIncomingRTPPkt(*sourceLink, source_created,
        network_address, transport_port) &&
        recordReception(*sourceLink,*packet,recvtime) ) {
        // now the packet link is linked in the queues
        IncomingRTPPktLink* packetLink = new IncomingRTPPktLink(packet, sourceLink, recvtime,
                                       packet->getTimestamp() - sourceLink->getInitialDataTimestamp(),
                                       NULL,NULL,NULL,NULL);
        insertRecvPacket(packetLink);
    } else {
        // must be discarded due to collision or loop or
        // invalid source
        delete packet;
        return 0;
    }
    // Start the ZRTP engine after we got a at least one RTP packet and
    // sent some as well or we are in multi-stream mode.
    if (!started && enableZrtp) {
        startZrtp();
    }
    return rtn;
}