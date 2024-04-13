NumberFormatTest::TestMultiCurrencySign() {
    const char* DATA[][6] = {
        // the fields in the following test are:
        // locale,
        // currency pattern (with negative pattern),
        // currency number to be formatted,
        // currency format using currency symbol name, such as "$" for USD,
        // currency format using currency ISO name, such as "USD",
        // currency format using plural name, such as "US dollars".
        // for US locale
        {"en_US", "\\u00A4#,##0.00;-\\u00A4#,##0.00", "1234.56", "$1,234.56", "USD\\u00A01,234.56", "US dollars\\u00A01,234.56"},
        {"en_US", "\\u00A4#,##0.00;-\\u00A4#,##0.00", "-1234.56", "-$1,234.56", "-USD\\u00A01,234.56", "-US dollars\\u00A01,234.56"},
        {"en_US", "\\u00A4#,##0.00;-\\u00A4#,##0.00", "1", "$1.00", "USD\\u00A01.00", "US dollars\\u00A01.00"},
        // for CHINA locale
        {"zh_CN", "\\u00A4#,##0.00;(\\u00A4#,##0.00)", "1234.56", "\\uFFE51,234.56", "CNY\\u00A01,234.56", "\\u4EBA\\u6C11\\u5E01\\u00A01,234.56"},
        {"zh_CN", "\\u00A4#,##0.00;(\\u00A4#,##0.00)", "-1234.56", "(\\uFFE51,234.56)", "(CNY\\u00A01,234.56)", "(\\u4EBA\\u6C11\\u5E01\\u00A01,234.56)"},
        {"zh_CN", "\\u00A4#,##0.00;(\\u00A4#,##0.00)", "1", "\\uFFE51.00", "CNY\\u00A01.00", "\\u4EBA\\u6C11\\u5E01\\u00A01.00"}
    };

    const UChar doubleCurrencySign[] = {0xA4, 0xA4, 0};
    UnicodeString doubleCurrencyStr(doubleCurrencySign);
    const UChar tripleCurrencySign[] = {0xA4, 0xA4, 0xA4, 0};
    UnicodeString tripleCurrencyStr(tripleCurrencySign);

    for (uint32_t i=0; i<UPRV_LENGTHOF(DATA); ++i) {
        const char* locale = DATA[i][0];
        UnicodeString pat = ctou(DATA[i][1]);
        double numberToBeFormat = atof(DATA[i][2]);
        UErrorCode status = U_ZERO_ERROR;
        DecimalFormatSymbols* sym = new DecimalFormatSymbols(Locale(locale), status);
        if (U_FAILURE(status)) {
            delete sym;
            continue;
        }
        for (int j=1; j<=3; ++j) {
            // j represents the number of currency sign in the pattern.
            if (j == 2) {
                pat = pat.findAndReplace(ctou("\\u00A4"), doubleCurrencyStr);
            } else if (j == 3) {
                pat = pat.findAndReplace(ctou("\\u00A4\\u00A4"), tripleCurrencyStr);
            }

            DecimalFormat* fmt = new DecimalFormat(pat, new DecimalFormatSymbols(*sym), status);
            if (U_FAILURE(status)) {
                errln("FAILED init DecimalFormat ");
                delete fmt;
                continue;
            }
            UnicodeString s;
            ((NumberFormat*) fmt)->format(numberToBeFormat, s);
            // DATA[i][3] is the currency format result using a
            // single currency sign.
            // DATA[i][4] is the currency format result using
            // double currency sign.
            // DATA[i][5] is the currency format result using
            // triple currency sign.
            // DATA[i][j+2] is the currency format result using
            // 'j' number of currency sign.
            UnicodeString currencyFormatResult = ctou(DATA[i][2+j]);
            if (s.compare(currencyFormatResult)) {
                errln("FAIL format: Expected " + currencyFormatResult + "; Got " + s);
            }
            // mix style parsing
            for (int k=3; k<=5; ++k) {
              // DATA[i][3] is the currency format result using a
              // single currency sign.
              // DATA[i][4] is the currency format result using
              // double currency sign.
              // DATA[i][5] is the currency format result using
              // triple currency sign.
              UnicodeString oneCurrencyFormat = ctou(DATA[i][k]);
              UErrorCode status = U_ZERO_ERROR;
              Formattable parseRes;
              fmt->parse(oneCurrencyFormat, parseRes, status);
              if (U_FAILURE(status) ||
                  (parseRes.getType() == Formattable::kDouble &&
                   parseRes.getDouble() != numberToBeFormat) ||
                  (parseRes.getType() == Formattable::kLong &&
                   parseRes.getLong() != numberToBeFormat)) {
                  errln("FAILED parse " + oneCurrencyFormat + "; (i, j, k): " +
                        i + ", " + j + ", " + k);
              }
            }
            delete fmt;
        }
        delete sym;
    }
}