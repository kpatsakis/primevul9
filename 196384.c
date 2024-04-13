void NumberFormatTest::Test13840_ParseLongStringCrash() {
    IcuTestErrorCode status(*this, "Test13840_ParseLongStringCrash");

    LocalPointer<NumberFormat> nf(NumberFormat::createInstance("en", status), status);
    if (status.errIfFailureAndReset()) { return; }

    Formattable result;
    static const char16_t* bigString =
        u"111111111111111111111111111111111111111111111111111111111111111111111"
        u"111111111111111111111111111111111111111111111111111111111111111111111"
        u"111111111111111111111111111111111111111111111111111111111111111111111"
        u"111111111111111111111111111111111111111111111111111111111111111111111"
        u"111111111111111111111111111111111111111111111111111111111111111111111"
        u"111111111111111111111111111111111111111111111111111111111111111111111";
    nf->parse(bigString, result, status);

    // Normalize the input string:
    CharString expectedChars;
    expectedChars.appendInvariantChars(bigString, status);
    DecimalQuantity expectedDQ;
    expectedDQ.setToDecNumber(expectedChars.toStringPiece(), status);
    UnicodeString expectedUString = expectedDQ.toScientificString();

    // Get the output string:
    StringPiece actualChars = result.getDecimalNumber(status);
    UnicodeString actualUString = UnicodeString(actualChars.data(), actualChars.length(), US_INV);

    assertEquals("Should round-trip without crashing", expectedUString, actualUString);
}