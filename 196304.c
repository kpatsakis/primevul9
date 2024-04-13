void NumberFormatTest::TestParseSignsAndMarks() {
    const SignsAndMarksItem items[] = {
        // locale               lenient numString                                                       value
        { "en",                 FALSE,  CharsToUnicodeString("12"),                                      12 },
        { "en",                 TRUE,   CharsToUnicodeString("12"),                                      12 },
        { "en",                 FALSE,  CharsToUnicodeString("-23"),                                    -23 },
        { "en",                 TRUE,   CharsToUnicodeString("-23"),                                    -23 },
        { "en",                 TRUE,   CharsToUnicodeString("- 23"),                                   -23 },
        { "en",                 FALSE,  CharsToUnicodeString("\\u200E-23"),                             -23 },
        { "en",                 TRUE,   CharsToUnicodeString("\\u200E-23"),                             -23 },
        { "en",                 TRUE,   CharsToUnicodeString("\\u200E- 23"),                            -23 },

        { "en@numbers=arab",    FALSE,  CharsToUnicodeString("\\u0663\\u0664"),                          34 },
        { "en@numbers=arab",    TRUE,   CharsToUnicodeString("\\u0663\\u0664"),                          34 },
        { "en@numbers=arab",    FALSE,  CharsToUnicodeString("-\\u0664\\u0665"),                        -45 },
        { "en@numbers=arab",    TRUE,   CharsToUnicodeString("-\\u0664\\u0665"),                        -45 },
        { "en@numbers=arab",    TRUE,   CharsToUnicodeString("- \\u0664\\u0665"),                       -45 },
        { "en@numbers=arab",    FALSE,  CharsToUnicodeString("\\u200F-\\u0664\\u0665"),                 -45 },
        { "en@numbers=arab",    TRUE,   CharsToUnicodeString("\\u200F-\\u0664\\u0665"),                 -45 },
        { "en@numbers=arab",    TRUE,   CharsToUnicodeString("\\u200F- \\u0664\\u0665"),                -45 },

        { "en@numbers=arabext", FALSE,  CharsToUnicodeString("\\u06F5\\u06F6"),                          56 },
        { "en@numbers=arabext", TRUE,   CharsToUnicodeString("\\u06F5\\u06F6"),                          56 },
        { "en@numbers=arabext", FALSE,  CharsToUnicodeString("-\\u06F6\\u06F7"),                        -67 },
        { "en@numbers=arabext", TRUE,   CharsToUnicodeString("-\\u06F6\\u06F7"),                        -67 },
        { "en@numbers=arabext", TRUE,   CharsToUnicodeString("- \\u06F6\\u06F7"),                       -67 },
        { "en@numbers=arabext", FALSE,  CharsToUnicodeString("\\u200E-\\u200E\\u06F6\\u06F7"),          -67 },
        { "en@numbers=arabext", TRUE,   CharsToUnicodeString("\\u200E-\\u200E\\u06F6\\u06F7"),          -67 },
        { "en@numbers=arabext", TRUE,   CharsToUnicodeString("\\u200E-\\u200E \\u06F6\\u06F7"),         -67 },

        { "he",                 FALSE,  CharsToUnicodeString("12"),                                      12 },
        { "he",                 TRUE,   CharsToUnicodeString("12"),                                      12 },
        { "he",                 FALSE,  CharsToUnicodeString("-23"),                                    -23 },
        { "he",                 TRUE,   CharsToUnicodeString("-23"),                                    -23 },
        { "he",                 TRUE,   CharsToUnicodeString("- 23"),                                   -23 },
        { "he",                 FALSE,  CharsToUnicodeString("\\u200E-23"),                             -23 },
        { "he",                 TRUE,   CharsToUnicodeString("\\u200E-23"),                             -23 },
        { "he",                 TRUE,   CharsToUnicodeString("\\u200E- 23"),                            -23 },

        { "ar",                 FALSE,  CharsToUnicodeString("\\u0663\\u0664"),                          34 },
        { "ar",                 TRUE,   CharsToUnicodeString("\\u0663\\u0664"),                          34 },
        { "ar",                 FALSE,  CharsToUnicodeString("-\\u0664\\u0665"),                        -45 },
        { "ar",                 TRUE,   CharsToUnicodeString("-\\u0664\\u0665"),                        -45 },
        { "ar",                 TRUE,   CharsToUnicodeString("- \\u0664\\u0665"),                       -45 },
        { "ar",                 FALSE,  CharsToUnicodeString("\\u200F-\\u0664\\u0665"),                 -45 },
        { "ar",                 TRUE,   CharsToUnicodeString("\\u200F-\\u0664\\u0665"),                 -45 },
        { "ar",                 TRUE,   CharsToUnicodeString("\\u200F- \\u0664\\u0665"),                -45 },

        { "ar_MA",              FALSE,  CharsToUnicodeString("12"),                                      12 },
        { "ar_MA",              TRUE,   CharsToUnicodeString("12"),                                      12 },
        { "ar_MA",              FALSE,  CharsToUnicodeString("-23"),                                    -23 },
        { "ar_MA",              TRUE,   CharsToUnicodeString("-23"),                                    -23 },
        { "ar_MA",              TRUE,   CharsToUnicodeString("- 23"),                                   -23 },
        { "ar_MA",              FALSE,  CharsToUnicodeString("\\u200E-23"),                             -23 },
        { "ar_MA",              TRUE,   CharsToUnicodeString("\\u200E-23"),                             -23 },
        { "ar_MA",              TRUE,   CharsToUnicodeString("\\u200E- 23"),                            -23 },

        { "fa",                 FALSE,  CharsToUnicodeString("\\u06F5\\u06F6"),                          56 },
        { "fa",                 TRUE,   CharsToUnicodeString("\\u06F5\\u06F6"),                          56 },
        { "fa",                 FALSE,  CharsToUnicodeString("\\u2212\\u06F6\\u06F7"),                  -67 },
        { "fa",                 TRUE,   CharsToUnicodeString("\\u2212\\u06F6\\u06F7"),                  -67 },
        { "fa",                 TRUE,   CharsToUnicodeString("\\u2212 \\u06F6\\u06F7"),                 -67 },
        { "fa",                 FALSE,  CharsToUnicodeString("\\u200E\\u2212\\u200E\\u06F6\\u06F7"),    -67 },
        { "fa",                 TRUE,   CharsToUnicodeString("\\u200E\\u2212\\u200E\\u06F6\\u06F7"),    -67 },
        { "fa",                 TRUE,   CharsToUnicodeString("\\u200E\\u2212\\u200E \\u06F6\\u06F7"),   -67 },

        { "ps",                 FALSE,  CharsToUnicodeString("\\u06F5\\u06F6"),                          56 },
        { "ps",                 TRUE,   CharsToUnicodeString("\\u06F5\\u06F6"),                          56 },
        { "ps",                 FALSE,  CharsToUnicodeString("-\\u06F6\\u06F7"),                        -67 },
        { "ps",                 TRUE,   CharsToUnicodeString("-\\u06F6\\u06F7"),                        -67 },
        { "ps",                 TRUE,   CharsToUnicodeString("- \\u06F6\\u06F7"),                       -67 },
        { "ps",                 FALSE,  CharsToUnicodeString("\\u200E-\\u200E\\u06F6\\u06F7"),          -67 },
        { "ps",                 TRUE,   CharsToUnicodeString("\\u200E-\\u200E\\u06F6\\u06F7"),          -67 },
        { "ps",                 TRUE,   CharsToUnicodeString("\\u200E-\\u200E \\u06F6\\u06F7"),         -67 },
        { "ps",                 FALSE,  CharsToUnicodeString("-\\u200E\\u06F6\\u06F7"),                 -67 },
        { "ps",                 TRUE,   CharsToUnicodeString("-\\u200E\\u06F6\\u06F7"),                 -67 },
        { "ps",                 TRUE,   CharsToUnicodeString("-\\u200E \\u06F6\\u06F7"),                -67 },
        // terminator
        { NULL,                 0,      UnicodeString(""),                                                0 },
    };

    const SignsAndMarksItem * itemPtr;
    for (itemPtr = items; itemPtr->locale != NULL; itemPtr++ ) {
        UErrorCode status = U_ZERO_ERROR;
        NumberFormat *numfmt = NumberFormat::createInstance(Locale(itemPtr->locale), status);
        if (U_SUCCESS(status)) {
            numfmt->setLenient(itemPtr->lenient);
            Formattable fmtobj;
            ParsePosition ppos;
            numfmt->parse(itemPtr->numString, fmtobj, ppos);
            if (ppos.getIndex() == itemPtr->numString.length()) {
                double parsedValue = fmtobj.getDouble(status);
                if (U_FAILURE(status) || parsedValue != itemPtr->value) {
                    errln((UnicodeString)"FAIL: locale " + itemPtr->locale + ", lenient " + itemPtr->lenient + ", parse of \"" + itemPtr->numString + "\" gives value " + parsedValue);
                }
            } else {
                errln((UnicodeString)"FAIL: locale " + itemPtr->locale + ", lenient " + itemPtr->lenient + ", parse of \"" + itemPtr->numString + "\" gives position " + ppos.getIndex());
            }
        } else {
            dataerrln("FAIL: NumberFormat::createInstance for locale % gives error %s", itemPtr->locale, u_errorName(status));
        }
        delete numfmt;
    }
}