void NumberFormatTest::Test10468ApplyPattern() {
    // Padding char of fmt is now 'a'
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat fmt("'I''ll'*a###.##", status);

    if (U_FAILURE(status)) {
        errcheckln(status, "DecimalFormat constructor failed - %s", u_errorName(status));
        return;
    }

    assertEquals("Padding character should be 'a'.", u"a", fmt.getPadCharacterString());

    // Padding char of fmt ought to be '*' since that is the default and no
    // explicit padding char is specified in the new pattern.
    fmt.applyPattern("AA#,##0.00ZZ", status);

    // Oops this still prints 'a' even though we changed the pattern.
    assertEquals("applyPattern did not clear padding character.", u" ", fmt.getPadCharacterString());
}