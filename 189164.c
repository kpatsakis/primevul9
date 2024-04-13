void ZrtpQueue::init()
{
    zrtpUserCallback = NULL;
    enableZrtp = false;
    started = false;
    mitmMode = false;
    enableParanoidMode = false;
    zrtpEngine = NULL;
    senderZrtpSeqNo = 1;

    clientIdString = clientId;
    peerSSRC = 0;
}