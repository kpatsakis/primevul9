void NumberFormatTest::TestAdoptDecimalFormatSymbols(void) {
    UErrorCode ec = U_ZERO_ERROR;
    DecimalFormatSymbols *sym = new DecimalFormatSymbols(Locale::getUS(), ec);
    if (U_FAILURE(ec)) {
        errcheckln(ec, "Fail: DecimalFormatSymbols constructor - %s", u_errorName(ec));
        delete sym;
        return;
    }
    UnicodeString pat(" #,##0.00");
    pat.insert(0, (UChar)0x00A4);
    DecimalFormat fmt(pat, sym, ec);
    if (U_FAILURE(ec)) {
        errln("Fail: DecimalFormat constructor");
        return;
    }

    UnicodeString str;
    fmt.format(2350.75, str);
    if (str == "$ 2,350.75") {
        logln(str);
    } else {
        dataerrln("Fail: " + str + ", expected $ 2,350.75");
    }

    sym = new DecimalFormatSymbols(Locale::getUS(), ec);
    if (U_FAILURE(ec)) {
        errln("Fail: DecimalFormatSymbols constructor");
        delete sym;
        return;
    }
    sym->setSymbol(DecimalFormatSymbols::kCurrencySymbol, "Q");
    fmt.adoptDecimalFormatSymbols(sym);

    str.truncate(0);
    fmt.format(2350.75, str);
    if (str == "Q 2,350.75") {
        logln(str);
    } else {
        dataerrln("Fail: adoptDecimalFormatSymbols -> " + str + ", expected Q 2,350.75");
    }

    sym = new DecimalFormatSymbols(Locale::getUS(), ec);
    if (U_FAILURE(ec)) {
        errln("Fail: DecimalFormatSymbols constructor");
        delete sym;
        return;
    }
    DecimalFormat fmt2(pat, sym, ec);
    if (U_FAILURE(ec)) {
        errln("Fail: DecimalFormat constructor");
        return;
    }

    DecimalFormatSymbols sym2(Locale::getUS(), ec);
    if (U_FAILURE(ec)) {
        errln("Fail: DecimalFormatSymbols constructor");
        return;
    }
    sym2.setSymbol(DecimalFormatSymbols::kCurrencySymbol, "Q");
    fmt2.setDecimalFormatSymbols(sym2);

    str.truncate(0);
    fmt2.format(2350.75, str);
    if (str == "Q 2,350.75") {
        logln(str);
    } else {
        dataerrln("Fail: setDecimalFormatSymbols -> " + str + ", expected Q 2,350.75");
    }
}