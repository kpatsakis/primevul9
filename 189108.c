int32_t ZrtpPacketHello::getVersionInt() {
    uint8_t* vp = getVersion();
    int32_t version = 0;

    if (isdigit(*vp) && isdigit(*vp+2)) {
        version = (*vp - '0') * 10;
        version += *(vp+2) - '0';
    }
    return version;
}