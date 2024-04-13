void DecimalQuantity::clear() {
    lOptPos = INT32_MAX;
    lReqPos = 0;
    rReqPos = 0;
    rOptPos = INT32_MIN;
    flags = 0;
    setBcdToZero(); // sets scale, precision, hasDouble, origDouble, origDelta, and BCD data
}