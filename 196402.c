void NumberFormatTest::TestHostClone()
{
    /*
    Verify that a cloned formatter gives the same results
    and is useable after the original has been deleted.
    */
    // This is mainly important on Windows.
    UErrorCode status = U_ZERO_ERROR;
    Locale loc("en_US@compat=host");
    UDate now = Calendar::getNow();
    NumberFormat *full = NumberFormat::createInstance(loc, status);
    if (full == NULL || U_FAILURE(status)) {
        dataerrln("FAIL: Can't create NumberFormat date instance - %s", u_errorName(status));
        return;
    }
    UnicodeString result1;
    full->format(now, result1, status);
    Format *fullClone = full->clone();
    delete full;
    full = NULL;

    UnicodeString result2;
    fullClone->format(now, result2, status);
    if (U_FAILURE(status)) {
        errln("FAIL: format failure.");
    }
    if (result1 != result2) {
        errln("FAIL: Clone returned different result from non-clone.");
    }
    delete fullClone;
}