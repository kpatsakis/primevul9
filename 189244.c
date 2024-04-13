bool ZrtpQueue::sendSASRelayPacket(uint8_t* sh, std::string render) {

    if (zrtpEngine != NULL)
        return zrtpEngine->sendSASRelayPacket(sh, render);
    else
        return false;
}