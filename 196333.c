UnicodeString DecimalQuantity::toPlainString() const {
    U_ASSERT(!isApproximate);
    UnicodeString sb;
    if (isNegative()) {
        sb.append(u'-');
    }
    if (precision == 0 || getMagnitude() < 0) {
        sb.append(u'0');
    }
    for (int m = getUpperDisplayMagnitude(); m >= getLowerDisplayMagnitude(); m--) {
        if (m == -1) { sb.append(u'.'); }
        sb.append(getDigit(m) + u'0');
    }
    return sb;
}