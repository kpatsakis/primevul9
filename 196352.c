void NumberFormatTest::TestAvailableNumberingSystems() {
    UErrorCode status = U_ZERO_ERROR;
    StringEnumeration *availableNumberingSystems = NumberingSystem::getAvailableNames(status);
    CHECK_DATA(status, "NumberingSystem::getAvailableNames()")

    int32_t nsCount = availableNumberingSystems->count(status);
    if ( nsCount < 74 ) {
        errln("FAIL: Didn't get as many numbering systems as we had hoped for. Need at least 74, got %d",nsCount);
    }

    /* A relatively simple test of the API.  We call getAvailableNames() and cycle through */
    /* each name returned, attempting to create a numbering system based on that name and  */
    /* verifying that the name returned from the resulting numbering system is the same    */
    /* one that we initially thought.                                                      */

    int32_t len;
    for ( int32_t i = 0 ; i < nsCount ; i++ ) {
        const char *nsname = availableNumberingSystems->next(&len,status);
        NumberingSystem* ns = NumberingSystem::createInstanceByName(nsname,status);
        logln("OK for ns = %s",nsname);
        if ( uprv_strcmp(nsname,ns->getName()) ) {
            errln("FAIL: Numbering system name didn't match for name = %s\n",nsname);
        }

        delete ns;
    }

    delete availableNumberingSystems;
}