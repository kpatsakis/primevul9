NumberFormatTest::TestCurrencyParsing() {
    static const char* DATA[][6] = {
        // the data are:
        // locale,
        // currency amount to be formatted,
        // currency ISO code to be formatted,
        // format result using CURRENCYSTYLE,
        // format result using ISOCURRENCYSTYLE,
        // format result using PLURALCURRENCYSTYLE,
        {"en_US", "1", "USD", "$1.00", "USD\\u00A01.00", "1.00 US dollars"},
        {"pa_IN", "1", "USD", "US$\\u00A01.00", "USD\\u00A01.00", "1.00 \\u0a2f\\u0a42.\\u0a10\\u0a38. \\u0a21\\u0a3e\\u0a32\\u0a30"},
        {"es_AR", "1", "USD", "US$\\u00A01,00", "USD\\u00A01,00", "1,00 d\\u00f3lar estadounidense"},
        {"ar_EG", "1", "USD", "\\u0661\\u066b\\u0660\\u0660\\u00a0US$", "\\u0661\\u066b\\u0660\\u0660\\u00a0USD", "\\u0661\\u066b\\u0660\\u0660 \\u062f\\u0648\\u0644\\u0627\\u0631 \\u0623\\u0645\\u0631\\u064a\\u0643\\u064a"},
        {"fa_CA", "1", "USD", "\\u200e$\\u06f1\\u066b\\u06f0\\u06f0", "\\u200eUSD\\u06f1\\u066b\\u06f0\\u06f0", "\\u06f1\\u066b\\u06f0\\u06f0 \\u062f\\u0644\\u0627\\u0631 \\u0627\\u0645\\u0631\\u06cc\\u06a9\\u0627"},
        {"he_IL", "1", "USD", "\\u200f1.00\\u00a0$", "\\u200f1.00\\u00a0USD", "1.00 \\u05d3\\u05d5\\u05dc\\u05e8 \\u05d0\\u05de\\u05e8\\u05d9\\u05e7\\u05d0\\u05d9"},
        {"hr_HR", "1", "USD", "1,00\\u00a0USD", "1,00\\u00a0USD", "1,00 ameri\\u010Dkih dolara"},
        {"id_ID", "1", "USD", "US$\\u00A01,00", "USD\\u00A01,00", "1,00 Dolar Amerika Serikat"},
        {"it_IT", "1", "USD", "1,00\\u00a0USD", "1,00\\u00a0USD", "1,00 dollari statunitensi"},
        {"ko_KR", "1", "USD", "US$\\u00A01.00", "USD\\u00A01.00", "1.00 \\ubbf8\\uad6d \\ub2ec\\ub7ec"},
        {"ja_JP", "1", "USD", "$1.00", "USD\\u00A01.00", "1.00\\u00A0\\u7c73\\u30c9\\u30eb"},
        {"zh_CN", "1", "CNY", "\\uFFE51.00", "CNY\\u00A001.00", "1.00\\u00A0\\u4EBA\\u6C11\\u5E01"},
        {"zh_TW", "1", "CNY", "CN\\u00A51.00", "CNY\\u00A01.00", "1.00 \\u4eba\\u6c11\\u5e63"},
        {"zh_Hant", "1", "CNY", "CN\\u00A51.00", "CNY\\u00A01.00", "1.00 \\u4eba\\u6c11\\u5e63"},
        {"zh_Hant", "1", "JPY", "\\u00A51.00", "JPY\\u00A01.00", "1 \\u65E5\\u5713"},
        {"ja_JP", "1", "JPY", "\\uFFE51.00", "JPY\\u00A01.00", "1\\u00A0\\u5186"},
        // ICU 62 requires #parseCurrency() to recognize variants when parsing
        // {"ja_JP", "1", "JPY", "\\u00A51.00", "JPY\\u00A01.00", "1\\u00A0\\u5186"},
        {"ru_RU", "1", "RUB", "1,00\\u00A0\\u00A0\\u20BD", "1,00\\u00A0\\u00A0RUB", "1,00 \\u0440\\u043E\\u0441\\u0441\\u0438\\u0439\\u0441\\u043A\\u043E\\u0433\\u043E \\u0440\\u0443\\u0431\\u043B\\u044F"}
    };
    static const UNumberFormatStyle currencyStyles[] = {
        UNUM_CURRENCY,
        UNUM_CURRENCY_ISO,
        UNUM_CURRENCY_PLURAL
    };
    static const char* currencyStyleNames[] = {
      "UNUM_CURRENCY",
      "UNUM_CURRENCY_ISO",
      "UNUM_CURRENCY_PLURAL"
    };

#ifdef NUMFMTST_CACHE_DEBUG
int deadloop = 0;
for (;;) {
    printf("loop: %d\n", deadloop++);
#endif
    for (uint32_t i=0; i< UPRV_LENGTHOF(DATA); ++i) {  /* i = test case #  - should be i=0*/
      for (int32_t kIndex = 2; kIndex < UPRV_LENGTHOF(currencyStyles); ++kIndex) {
        UNumberFormatStyle k = currencyStyles[kIndex]; /* k = style */
        const char* localeString = DATA[i][0];
        double numberToBeFormat = atof(DATA[i][1]);
        const char* currencyISOCode = DATA[i][2];
        Locale locale(localeString);
        UErrorCode status = U_ZERO_ERROR;
        NumberFormat* numFmt = NumberFormat::createInstance(locale, k, status);
        logln("#%d NumberFormat(%s, %s) Currency=%s\n",
              i, localeString, currencyStyleNames[kIndex],
              currencyISOCode);

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
            logln("parse(%s)", DATA[i][j]);
            numFmt->parse(oneCurrencyFormatResult, parseResult, status);
            if (U_FAILURE(status) ||
                (parseResult.getType() == Formattable::kDouble &&
                 parseResult.getDouble() != numberToBeFormat) ||
                (parseResult.getType() == Formattable::kLong &&
                 parseResult.getLong() != numberToBeFormat)) {
                errln((UnicodeString)"FAIL: NumberFormat(" + localeString +", " + currencyStyleNames[kIndex] +
                      "), Currency="+currencyISOCode+", parse("+DATA[i][j]+") returned error " + (UnicodeString)u_errorName(status)+".  Testcase: data[" + i + "][" + currencyStyleNames[j-3] +"="+j+"]");
                if (parseResult.getType() == Formattable::kDouble) {
                    errln((UnicodeString)"expected: " + numberToBeFormat + "; actual (double): " +parseResult.getDouble());
                } else {
                    errln((UnicodeString)"expected: " + numberToBeFormat + "; actual (long): " +parseResult.getLong());
                }
                errln((UnicodeString)" round-trip would be: " + strBuf);
            }
        }
        delete numFmt;
      }
    }
#ifdef NUMFMTST_CACHE_DEBUG
}
#endif
}