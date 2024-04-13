void NumberFormatTest::TestPatterns2(void) {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols US(Locale::getUS(), status);
    CHECK(status, "DecimalFormatSymbols constructor");

    DecimalFormat fmt("#", US, status);
    CHECK(status, "DecimalFormat constructor");

    UChar hat = 0x005E; /*^*/

    expectPad(fmt, "*^#", DecimalFormat::kPadBeforePrefix, 1, hat);
    expectPad(fmt, "$*^#", DecimalFormat::kPadAfterPrefix, 2, hat);
    expectPad(fmt, "#*^", DecimalFormat::kPadBeforeSuffix, 1, hat);
    expectPad(fmt, "#$*^", DecimalFormat::kPadAfterSuffix, 2, hat);
    expectPad(fmt, "$*^$#", ILLEGAL);
    expectPad(fmt, "#$*^$", ILLEGAL);
    expectPad(fmt, "'pre'#,##0*x'post'", DecimalFormat::kPadBeforeSuffix,
              12, (UChar)0x0078 /*x*/);
    expectPad(fmt, "''#0*x", DecimalFormat::kPadBeforeSuffix,
              3, (UChar)0x0078 /*x*/);
    expectPad(fmt, "'I''ll'*a###.##", DecimalFormat::kPadAfterPrefix,
              10, (UChar)0x0061 /*a*/);

    fmt.applyPattern("AA#,##0.00ZZ", status);
    CHECK(status, "applyPattern");
    fmt.setPadCharacter(hat);

    fmt.setFormatWidth(10);

    fmt.setPadPosition(DecimalFormat::kPadBeforePrefix);
    expectPat(fmt, "*^AA#,##0.00ZZ");

    fmt.setPadPosition(DecimalFormat::kPadBeforeSuffix);
    expectPat(fmt, "AA#,##0.00*^ZZ");

    fmt.setPadPosition(DecimalFormat::kPadAfterSuffix);
    expectPat(fmt, "AA#,##0.00ZZ*^");

    //            12  3456789012
    UnicodeString exp("AA*^#,##0.00ZZ", "");
    fmt.setFormatWidth(12);
    fmt.setPadPosition(DecimalFormat::kPadAfterPrefix);
    expectPat(fmt, exp);

    fmt.setFormatWidth(13);
    //              12  34567890123
    expectPat(fmt, "AA*^##,##0.00ZZ");

    fmt.setFormatWidth(14);
    //              12  345678901234
    expectPat(fmt, "AA*^###,##0.00ZZ");

    fmt.setFormatWidth(15);
    //              12  3456789012345
    expectPat(fmt, "AA*^####,##0.00ZZ"); // This is the interesting case

    fmt.setFormatWidth(16);
    //              12  34567890123456
    expectPat(fmt, "AA*^#####,##0.00ZZ");
}