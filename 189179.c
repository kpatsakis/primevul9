std::string ZRtp::getPeerClientId() {
    if (peerClientId.empty())
        return std::string();
    return peerClientId;
}