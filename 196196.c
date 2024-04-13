static void setError(UErrorCode& ec, UErrorCode err) {
    if (U_SUCCESS(ec)) {
        ec = err;
    }
}