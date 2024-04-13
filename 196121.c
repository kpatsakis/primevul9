NumberFormatTest::TestSpaceParsing() {
    // the data are:
    // the string to be parsed, parsed position, parsed error index
    const TestSpaceParsingItem DATA[] = {
        {"$124",           4, -1, FALSE},
        {"$124 $124",      4, -1, FALSE},
        {"$124 ",          4, -1, FALSE},
        {"$ 124 ",         0,  1, FALSE},
        {"$\\u00A0124 ",   5, -1, FALSE},
        {" $ 124 ",        0,  0, FALSE},
        {"124$",           0,  4, FALSE},
        {"124 $",          0,  3, FALSE},
        {"$124",           4, -1, TRUE},
        {"$124 $124",      4, -1, TRUE},
        {"$124 ",          4, -1, TRUE},
        {"$ 124 ",         5, -1, TRUE},
        {"$\\u00A0124 ",   5, -1, TRUE},
        {" $ 124 ",        6, -1, TRUE},
        {"124$",           4, -1, TRUE},
        {"124$",           4, -1, TRUE},
        {"124 $",          5, -1, TRUE},
        {"124 $",          5, -1, TRUE},
    };
    UErrorCode status = U_ZERO_ERROR;
    Locale locale("en_US");
    NumberFormat* foo = NumberFormat::createCurrencyInstance(locale, status);

    if (U_FAILURE(status)) {
        delete foo;
        return;
    }
    for (uint32_t i = 0; i < UPRV_LENGTHOF(DATA); ++i) {
        ParsePosition parsePosition(0);
        UnicodeString stringToBeParsed = ctou(DATA[i].stringToParse);
        int parsedPosition = DATA[i].parsedPos;
        int errorIndex = DATA[i].errorIndex;
        foo->setLenient(DATA[i].lenient);
        Formattable result;
        foo->parse(stringToBeParsed, result, parsePosition);
        logln("Parsing: " + stringToBeParsed);
        if (parsePosition.getIndex() != parsedPosition ||
            parsePosition.getErrorIndex() != errorIndex) {
            errln("FAILED parse " + stringToBeParsed + "; lenient: " + DATA[i].lenient + "; wrong position, expected: (" + parsedPosition + ", " + errorIndex + "); got (" + parsePosition.getIndex() + ", " + parsePosition.getErrorIndex() + ")");
        }
        if (parsePosition.getErrorIndex() == -1 &&
            result.getType() == Formattable::kLong &&
            result.getLong() != 124) {
            errln("FAILED parse " + stringToBeParsed + "; wrong number, expect: 124, got " + result.getLong());
        }
    }
    delete foo;
}