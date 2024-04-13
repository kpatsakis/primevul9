RateTransposer::RateTransposer() : FIFOProcessor(&outputBuffer)
{
    bUseAAFilter = 
#ifndef SOUNDTOUCH_PREVENT_CLICK_AT_RATE_CROSSOVER
        true;
#else
        // Disable Anti-alias filter if desirable to avoid click at rate change zero value crossover
        false;
#endif

    // Instantiates the anti-alias filter
    pAAFilter = new AAFilter(64);
    pTransposer = TransposerBase::newInstance();
}