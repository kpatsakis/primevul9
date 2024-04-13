bool DecimalQuantity::operator==(const DecimalQuantity& other) const {
    bool basicEquals =
            scale == other.scale
            && precision == other.precision
            && flags == other.flags
            && lOptPos == other.lOptPos
            && lReqPos == other.lReqPos
            && rReqPos == other.rReqPos
            && rOptPos == other.rOptPos
            && isApproximate == other.isApproximate;
    if (!basicEquals) {
        return false;
    }

    if (precision == 0) {
        return true;
    } else if (isApproximate) {
        return origDouble == other.origDouble && origDelta == other.origDelta;
    } else {
        for (int m = getUpperDisplayMagnitude(); m >= getLowerDisplayMagnitude(); m--) {
            if (getDigit(m) != other.getDigit(m)) {
                return false;
            }
        }
        return true;
    }
}