ZrtpStateClass::ZrtpStateClass(ZRtp *p) : parent(p), commitPkt(NULL), multiStream(false), secSubstate(Normal), sentVersion(0) {
    engine = new ZrtpStates(states, numberOfStates, Initial);

    // Set up timers according to ZRTP spec
    T1.start = 50;
    T1.maxResend = 20;
    T1.capping = 200;

    T2.start = 150;
    T2.maxResend = 10;
    T2.capping = 600;
}