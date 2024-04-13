std::string ZRtp::getHelloHash(int32_t index) {
    std::ostringstream stm;

    if (index < 0 || index >= MAX_ZRTP_VERSIONS)
        return std::string();

    uint8_t* hp = helloPackets[index].helloHash;

    char version[5] = {'\0'};
    strncpy(version, (const char*)helloPackets[index].packet->getVersion(), ZRTP_WORD_SIZE);

    stm << version;
    stm << " ";
    stm.fill('0');
    stm << hex;
    for (int i = 0; i < hashLengthImpl; i++) {
        stm.width(2);
        stm << static_cast<uint32_t>(*hp++);
    }
    return stm.str();
}