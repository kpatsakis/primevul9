void NumberFormatTest::TestPad(void) {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols US(Locale::getUS(), status);
    CHECK(status, "DecimalFormatSymbols constructor");

    expect2(new DecimalFormat("*^##.##", US, status),
           int32_t(0), "^^^^0", status);
    expect2(new DecimalFormat("*^##.##", US, status),
           -1.3, "^-1.3", status);
    expect2(new DecimalFormat("##0.0####E0*_ 'g-m/s^2'", US, status),
           int32_t(0), "0.0E0______ g-m/s^2", status);
    expect(new DecimalFormat("##0.0####E0*_ 'g-m/s^2'", US, status),
           1.0/3, "333.333E-3_ g-m/s^2", status);
    expect2(new DecimalFormat("##0.0####*_ 'g-m/s^2'", US, status),
           int32_t(0), "0.0______ g-m/s^2", status);
    expect(new DecimalFormat("##0.0####*_ 'g-m/s^2'", US, status),
           1.0/3, "0.33333__ g-m/s^2", status);

    // Test padding before a sign
    const char *formatStr = "*x#,###,###,##0.0#;*x(###,###,##0.0#)";
    expect2(new DecimalFormat(formatStr, US, status),
           int32_t(-10),  "xxxxxxxxxx(10.0)", status);
    expect2(new DecimalFormat(formatStr, US, status),
           int32_t(-1000),"xxxxxxx(1,000.0)", status);
    expect2(new DecimalFormat(formatStr, US, status),
           int32_t(-1000000),"xxx(1,000,000.0)", status);
    expect2(new DecimalFormat(formatStr, US, status),
           -100.37,       "xxxxxxxx(100.37)", status);
    expect2(new DecimalFormat(formatStr, US, status),
           -10456.37,     "xxxxx(10,456.37)", status);
    expect2(new DecimalFormat(formatStr, US, status),
           -1120456.37,   "xx(1,120,456.37)", status);
    expect2(new DecimalFormat(formatStr, US, status),
           -112045600.37, "(112,045,600.37)", status);
    expect2(new DecimalFormat(formatStr, US, status),
           -1252045600.37,"(1,252,045,600.37)", status);

    expect2(new DecimalFormat(formatStr, US, status),
           int32_t(10),  "xxxxxxxxxxxx10.0", status);
    expect2(new DecimalFormat(formatStr, US, status),
           int32_t(1000),"xxxxxxxxx1,000.0", status);
    expect2(new DecimalFormat(formatStr, US, status),
           int32_t(1000000),"xxxxx1,000,000.0", status);
    expect2(new DecimalFormat(formatStr, US, status),
           100.37,       "xxxxxxxxxx100.37", status);
    expect2(new DecimalFormat(formatStr, US, status),
           10456.37,     "xxxxxxx10,456.37", status);
    expect2(new DecimalFormat(formatStr, US, status),
           1120456.37,   "xxxx1,120,456.37", status);
    expect2(new DecimalFormat(formatStr, US, status),
           112045600.37, "xx112,045,600.37", status);
    expect2(new DecimalFormat(formatStr, US, status),
           10252045600.37,"10,252,045,600.37", status);


    // Test padding between a sign and a number
    const char *formatStr2 = "#,###,###,##0.0#*x;(###,###,##0.0#*x)";
    expect2(new DecimalFormat(formatStr2, US, status),
           int32_t(-10),  "(10.0xxxxxxxxxx)", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           int32_t(-1000),"(1,000.0xxxxxxx)", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           int32_t(-1000000),"(1,000,000.0xxx)", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           -100.37,       "(100.37xxxxxxxx)", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           -10456.37,     "(10,456.37xxxxx)", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           -1120456.37,   "(1,120,456.37xx)", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           -112045600.37, "(112,045,600.37)", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           -1252045600.37,"(1,252,045,600.37)", status);

    expect2(new DecimalFormat(formatStr2, US, status),
           int32_t(10),  "10.0xxxxxxxxxxxx", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           int32_t(1000),"1,000.0xxxxxxxxx", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           int32_t(1000000),"1,000,000.0xxxxx", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           100.37,       "100.37xxxxxxxxxx", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           10456.37,     "10,456.37xxxxxxx", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           1120456.37,   "1,120,456.37xxxx", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           112045600.37, "112,045,600.37xx", status);
    expect2(new DecimalFormat(formatStr2, US, status),
           10252045600.37,"10,252,045,600.37", status);

    //testing the setPadCharacter(UnicodeString) and getPadCharacterString()
    DecimalFormat fmt("#", US, status);
    CHECK(status, "DecimalFormat constructor");
    UnicodeString padString("P");
    fmt.setPadCharacter(padString);
    expectPad(fmt, "*P##.##", DecimalFormat::kPadBeforePrefix, 5, padString);
    fmt.setPadCharacter((UnicodeString)"^");
    expectPad(fmt, "*^#", DecimalFormat::kPadBeforePrefix, 1, (UnicodeString)"^");
    //commented untill implementation is complete
  /*  fmt.setPadCharacter((UnicodeString)"^^^");
    expectPad(fmt, "*^^^#", DecimalFormat::kPadBeforePrefix, 3, (UnicodeString)"^^^");
    padString.remove();
    padString.append((UChar)0x0061);
    padString.append((UChar)0x0302);
    fmt.setPadCharacter(padString);
    UChar patternChars[]={0x002a, 0x0061, 0x0302, 0x0061, 0x0302, 0x0023, 0x0000};
    UnicodeString pattern(patternChars);
    expectPad(fmt, pattern , DecimalFormat::kPadBeforePrefix, 4, padString);
 */

}