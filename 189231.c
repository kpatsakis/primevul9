ZrtpPacketHello::~ZrtpPacketHello() {
    DEBUGOUT((fprintf(stdout, "Deleting Hello packet: alloc: %x\n", allocated)));
}