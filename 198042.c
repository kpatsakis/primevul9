void TDStretch::setTempo(double newTempo)
{
    int intskip;

    tempo = newTempo;

    // Calculate new sequence duration
    calcSeqParameters();

    // Calculate ideal skip length (according to tempo value) 
    nominalSkip = tempo * (seekWindowLength - overlapLength);
    intskip = (int)(nominalSkip + 0.5);

    // Calculate how many samples are needed in the 'inputBuffer' to 
    // process another batch of samples
    //sampleReq = max(intskip + overlapLength, seekWindowLength) + seekLength / 2;
    sampleReq = max(intskip + overlapLength, seekWindowLength) + seekLength;
}