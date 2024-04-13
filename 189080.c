void ZrtpQueue::zrtpNegotiationFailed(MessageSeverity severity, int32_t subCode) {
    if (zrtpUserCallback != NULL) {
        zrtpUserCallback->zrtpNegotiationFailed(severity, subCode);
    }
}