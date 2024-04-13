NumberFormatTest::TestCurrencyFormatForMixParsing() {
    UErrorCode status = U_ZERO_ERROR;
    MeasureFormat* curFmt = MeasureFormat::createCurrencyFormat(Locale("en_US"), status);
    if (U_FAILURE(status)) {
        delete curFmt;
        return;
    }
    const char* formats[] = {
        "$1,234.56",  // string to be parsed
        "USD1,234.56",
        "US dollars1,234.56",
        // "1,234.56 US dollars" // Fails in 62 because currency format is not compatible with pattern.
    };
    const CurrencyAmount* curramt = NULL;
    for (uint32_t i = 0; i < UPRV_LENGTHOF(formats); ++i) {
        UnicodeString stringToBeParsed = ctou(formats[i]);
        logln(UnicodeString("stringToBeParsed: ") + stringToBeParsed);
        Formattable result;
        UErrorCode status = U_ZERO_ERROR;
        curFmt->parseObject(stringToBeParsed, result, status);
        if (U_FAILURE(status)) {
          errln("FAIL: measure format parsing: '%s' ec: %s", formats[i], u_errorName(status));
        } else if (result.getType() != Formattable::kObject ||
            (curramt = dynamic_cast<const CurrencyAmount*>(result.getObject())) == NULL ||
            curramt->getNumber().getDouble() != 1234.56 ||
            UnicodeString(curramt->getISOCurrency()).compare(ISO_CURRENCY_USD)
        ) {
            errln("FAIL: getCurrencyFormat of default locale (en_US) failed roundtripping the number ");
            if (curramt->getNumber().getDouble() != 1234.56) {
                errln((UnicodeString)"wong number, expect: 1234.56" + ", got: " + curramt->getNumber().getDouble());
            }
            if (curramt->getISOCurrency() != ISO_CURRENCY_USD) {
                errln((UnicodeString)"wong currency, expect: USD" + ", got: " + curramt->getISOCurrency());
            }
        }
    }
    delete curFmt;
}