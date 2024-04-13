void RateTransposer::enableAAFilter(bool newMode)
{
#ifndef SOUNDTOUCH_PREVENT_CLICK_AT_RATE_CROSSOVER
    // Disable Anti-alias filter if desirable to avoid click at rate change zero value crossover
    bUseAAFilter = newMode;
#endif
}