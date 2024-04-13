ZrtpQueue::onSRTPPacketError(IncomingRTPPkt& pkt, int32 errorCode)
{
    if (errorCode == -1) {
        sendInfo(Warning, WarningSRTPauthError);
    }
    else {
        sendInfo(Warning, WarningSRTPreplayError);
    }
    return false;
}