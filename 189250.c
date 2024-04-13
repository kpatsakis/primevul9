void ZRtp::zrtpNegotiationFailed(GnuZrtpCodes::MessageSeverity severity, int32_t subCode) {
    callback->zrtpNegotiationFailed(severity, subCode);
}