void NumberFormatTest::TestSecondaryGrouping(void) {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols US(Locale::getUS(), status);
    CHECK(status, "DecimalFormatSymbols ct");

    DecimalFormat f("#,##,###", US, status);
    CHECK(status, "DecimalFormat ct");

    expect2(f, (int32_t)123456789L, "12,34,56,789");
    expectPat(f, "#,##,##0");
    f.applyPattern("#,###", status);
    CHECK(status, "applyPattern");

    f.setSecondaryGroupingSize(4);
    expect2(f, (int32_t)123456789L, "12,3456,789");
    expectPat(f, "#,####,##0");
    NumberFormat *g = NumberFormat::createInstance(Locale("hi", "IN"), status);
    CHECK_DATA(status, "createInstance(hi_IN)");

    UnicodeString out;
    int32_t l = (int32_t)1876543210L;
    g->format(l, out);
    delete g;
    // expect "1,87,65,43,210", but with Hindi digits
    //         01234567890123
    UBool ok = TRUE;
    if (out.length() != 14) {
        ok = FALSE;
    } else {
        for (int32_t i=0; i<out.length(); ++i) {
            UBool expectGroup = FALSE;
            switch (i) {
            case 1:
            case 4:
            case 7:
            case 10:
                expectGroup = TRUE;
                break;
            }
            // Later -- fix this to get the actual grouping
            // character from the resource bundle.
            UBool isGroup = (out.charAt(i) == 0x002C);
            if (isGroup != expectGroup) {
                ok = FALSE;
                break;
            }
        }
    }
    if (!ok) {
        errln((UnicodeString)"FAIL  Expected " + l +
              " x hi_IN -> \"1,87,65,43,210\" (with Hindi digits), got \"" +
              escape(out) + "\"");
    } else {
        logln((UnicodeString)"Ok    " + l +
              " x hi_IN -> \"" +
              escape(out) + "\"");
    }
}