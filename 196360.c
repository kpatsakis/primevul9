void DecimalQuantity::convertToAccurateDouble() {
    U_ASSERT(origDouble != 0);
    int32_t delta = origDelta;

    // Call the slow oracle function (Double.toString in Java, DoubleToAscii in C++).
    char buffer[DoubleToStringConverter::kBase10MaximalLength + 1];
    bool sign; // unused; always positive
    int32_t length;
    int32_t point;
    DoubleToStringConverter::DoubleToAscii(
        origDouble,
        DoubleToStringConverter::DtoaMode::SHORTEST,
        0,
        buffer,
        sizeof(buffer),
        &sign,
        &length,
        &point
    );

    setBcdToZero();
    readDoubleConversionToBcd(buffer, length, point);
    scale += delta;
    explicitExactDouble = true;
}