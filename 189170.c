std::string ZRtp::getPeerProtcolVersion() {
    if (peerHelloVersion[0] == 0)
        return std::string();
    return std::string((char*)peerHelloVersion);
}