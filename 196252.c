void DecimalQuantity::copyFieldsFrom(const DecimalQuantity& other) {
    bogus = other.bogus;
    lOptPos = other.lOptPos;
    lReqPos = other.lReqPos;
    rReqPos = other.rReqPos;
    rOptPos = other.rOptPos;
    scale = other.scale;
    precision = other.precision;
    flags = other.flags;
    origDouble = other.origDouble;
    origDelta = other.origDelta;
    isApproximate = other.isApproximate;
}