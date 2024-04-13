void RateTransposer::setRate(double newRate)
{
    double fCutoff;

    pTransposer->setRate(newRate);

    // design a new anti-alias filter
    if (newRate > 1.0) 
    {
        fCutoff = 0.5 / newRate;
    } 
    else 
    {
        fCutoff = 0.5 * newRate;
    }
    pAAFilter->setCutoffFreq(fCutoff);
}