void ZrtpQueue::sendInfo(MessageSeverity severity, int32_t subCode) {
    if (zrtpUserCallback != NULL) {
        zrtpUserCallback->showMessage(severity, subCode);
    }
}