void NumberFormatTest::TestComplexCurrency() {

//    UErrorCode ec = U_ZERO_ERROR;
//    Locale loc("kn", "IN", "");
//    NumberFormat* fmt = NumberFormat::createCurrencyInstance(loc, ec);
//    if (U_SUCCESS(ec)) {
//        expect2(*fmt, 1.0, CharsToUnicodeString("Re.\\u00A01.00"));
//        Use .00392625 because that's 2^-8.  Any value less than 0.005 is fine.
//        expect(*fmt, 1.00390625, CharsToUnicodeString("Re.\\u00A01.00")); // tricky
//        expect2(*fmt, 12345678.0, CharsToUnicodeString("Rs.\\u00A01,23,45,678.00"));
//        expect2(*fmt, 0.5, CharsToUnicodeString("Rs.\\u00A00.50"));
//        expect2(*fmt, -1.0, CharsToUnicodeString("-Re.\\u00A01.00"));
//        expect2(*fmt, -10.0, CharsToUnicodeString("-Rs.\\u00A010.00"));
//    } else {
//        errln("FAIL: getCurrencyInstance(kn_IN)");
//    }
//    delete fmt;

}