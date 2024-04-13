bool ZRtp::setSignatureData(uint8_t* data, int32_t length) {
    if ((length % 4) != 0)
        return false;

    ZrtpPacketConfirm* cfrm = (myRole == Responder) ? &zrtpConfirm1 : &zrtpConfirm2;
    cfrm->setSignatureLength(length / 4);
    return cfrm->setSignatureData(data, length);
}