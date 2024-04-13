std::string ZRtp::getPeerHelloHash() {
    std::ostringstream stm;

    if (peerHelloVersion[0] == 0)
        return std::string();

    uint8_t* hp = peerHelloHash;

    stm << peerHelloVersion;
    stm << " ";
    stm.fill('0');
    stm << hex;
    for (int i = 0; i < hashLengthImpl; i++) {
        stm.width(2);
        stm << static_cast<uint32_t>(*hp++);
    }
    return stm.str();
}