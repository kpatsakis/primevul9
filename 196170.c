NumberFormatTest::TestCurrencyIsoPluralFormat() {
    static const char* DATA[][6] = {
        // the data are:
        // locale,
        // currency amount to be formatted,
        // currency ISO code to be formatted,
        // format result using CURRENCYSTYLE,
        // format result using ISOCURRENCYSTYLE,
        // format result using PLURALCURRENCYSTYLE,

        {"en_US", "1", "USD", "$1.00", "USD\\u00A01.00", "1.00 US dollars"},
        {"en_US", "1234.56", "USD", "$1,234.56", "USD\\u00A01,234.56", "1,234.56 US dollars"},
        {"en_US", "-1234.56", "USD", "-$1,234.56", "-USD\\u00A01,234.56", "-1,234.56 US dollars"},
        {"zh_CN", "1", "USD", "US$1.00", "USD\\u00A01.00", "1.00\\u00A0\\u7F8E\\u5143"},
        {"zh_CN", "1234.56", "USD", "US$1,234.56", "USD\\u00A01,234.56", "1,234.56\\u00A0\\u7F8E\\u5143"},
        {"zh_CN", "1", "CNY", "\\uFFE51.00", "CNY\\u00A01.00", "1.00\\u00A0\\u4EBA\\u6C11\\u5E01"},
        {"zh_CN", "1234.56", "CNY", "\\uFFE51,234.56", "CNY\\u00A01,234.56", "1,234.56\\u00A0\\u4EBA\\u6C11\\u5E01"},
        {"ru_RU", "1", "RUB", "1,00\\u00A0\\u20BD", "1,00\\u00A0RUB", "1,00 \\u0440\\u043E\\u0441\\u0441\\u0438\\u0439\\u0441\\u043A\\u043E\\u0433\\u043E \\u0440\\u0443\\u0431\\u043B\\u044F"},
        {"ru_RU", "2", "RUB", "2,00\\u00A0\\u20BD", "2,00\\u00A0RUB", "2,00 \\u0440\\u043E\\u0441\\u0441\\u0438\\u0439\\u0441\\u043A\\u043E\\u0433\\u043E \\u0440\\u0443\\u0431\\u043B\\u044F"},
        {"ru_RU", "5", "RUB", "5,00\\u00A0\\u20BD", "5,00\\u00A0RUB", "5,00 \\u0440\\u043E\\u0441\\u0441\\u0438\\u0439\\u0441\\u043A\\u043E\\u0433\\u043E \\u0440\\u0443\\u0431\\u043B\\u044F"},
        // test locale without currency information
        {"root", "-1.23", "USD", "-US$\\u00A01.23", "-USD\\u00A01.23", "-1.23 USD"},
        // test choice format
        {"es_AR", "1", "INR", "INR\\u00A01,00", "INR\\u00A01,00", "1,00 rupia india"},
    };
    static const UNumberFormatStyle currencyStyles[] = {
        UNUM_CURRENCY,
        UNUM_CURRENCY_ISO,
        UNUM_CURRENCY_PLURAL
    };

    for (int32_t i=0; i<UPRV_LENGTHOF(DATA); ++i) {
      const char* localeString = DATA[i][0];
      double numberToBeFormat = atof(DATA[i][1]);
      const char* currencyISOCode = DATA[i][2];
      logln(UnicodeString(u"Locale: ") + localeString + "; amount: " + numberToBeFormat);
      Locale locale(localeString);
      for (int32_t kIndex = 0; kIndex < UPRV_LENGTHOF(currencyStyles); ++kIndex) {
        UNumberFormatStyle k = currencyStyles[kIndex];
        logln(UnicodeString(u"UNumberFormatStyle: ") + k);
        UErrorCode status = U_ZERO_ERROR;
        NumberFormat* numFmt = NumberFormat::createInstance(locale, k, status);
        if (U_FAILURE(status)) {
            delete numFmt;
            dataerrln((UnicodeString)"can not create instance, locale:" + localeString + ", style: " + k + " - " + u_errorName(status));
            continue;
        }
        UChar currencyCode[4];
        u_charsToUChars(currencyISOCode, currencyCode, 4);
        numFmt->setCurrency(currencyCode, status);
        if (U_FAILURE(status)) {
            delete numFmt;
            errln((UnicodeString)"can not set currency:" + currencyISOCode);
            continue;
        }

        UnicodeString strBuf;
        numFmt->format(numberToBeFormat, strBuf);
        int resultDataIndex = 3 + kIndex;
        // DATA[i][resultDataIndex] is the currency format result
        // using 'k' currency style.
        UnicodeString formatResult = ctou(DATA[i][resultDataIndex]);
        if (strBuf.compare(formatResult)) {
            errln("FAIL: Expected " + formatResult + " actual: " + strBuf);
        }
        // test parsing, and test parsing for all currency formats.
        // NOTE: ICU 62 requires that the currency format match the pattern in strict mode.
        numFmt->setLenient(TRUE);
        for (int j = 3; j < 6; ++j) {
            // DATA[i][3] is the currency format result using
            // CURRENCYSTYLE formatter.
            // DATA[i][4] is the currency format result using
            // ISOCURRENCYSTYLE formatter.
            // DATA[i][5] is the currency format result using
            // PLURALCURRENCYSTYLE formatter.
            UnicodeString oneCurrencyFormatResult = ctou(DATA[i][j]);
            UErrorCode status = U_ZERO_ERROR;
            Formattable parseResult;
            numFmt->parse(oneCurrencyFormatResult, parseResult, status);
            if (U_FAILURE(status) ||
                (parseResult.getType() == Formattable::kDouble &&
                 parseResult.getDouble() != numberToBeFormat) ||
                (parseResult.getType() == Formattable::kLong &&
                 parseResult.getLong() != numberToBeFormat)) {
                errln((UnicodeString)"FAIL: getCurrencyFormat of locale " +
                      localeString + " failed roundtripping the number");
                if (parseResult.getType() == Formattable::kDouble) {
                    errln((UnicodeString)"expected: " + numberToBeFormat + "; actual: " +parseResult.getDouble());
                } else {
                    errln((UnicodeString)"expected: " + numberToBeFormat + "; actual: " +parseResult.getLong());
                }
            }
        }
        delete numFmt;
      }
    }
}