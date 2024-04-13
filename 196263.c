void NumberFormatTest::TestNumberingSystems() {

    const TestNumberingSystemItem DATA[] = {
        { "en_US@numbers=thai", 1234.567, FALSE, "\\u0E51,\\u0E52\\u0E53\\u0E54.\\u0E55\\u0E56\\u0E57" },
        { "en_US@numbers=hebr", 5678.0, TRUE, "\\u05D4\\u05F3\\u05EA\\u05E8\\u05E2\\u05F4\\u05D7" },
        { "en_US@numbers=arabext", 1234.567, FALSE, "\\u06F1\\u066c\\u06F2\\u06F3\\u06F4\\u066b\\u06F5\\u06F6\\u06F7" },
        { "ar_EG", 1234.567, FALSE, "\\u0661\\u066C\\u0662\\u0663\\u0664\\u066b\\u0665\\u0666\\u0667" },
        { "th_TH@numbers=traditional", 1234.567, FALSE, "\\u0E51,\\u0E52\\u0E53\\u0E54.\\u0E55\\u0E56\\u0E57" }, // fall back to native per TR35
        { "ar_MA", 1234.567, FALSE, "1.234,567" },
        { "en_US@numbers=hanidec", 1234.567, FALSE, "\\u4e00,\\u4e8c\\u4e09\\u56db.\\u4e94\\u516d\\u4e03" },
        { "ta_IN@numbers=native", 1234.567, FALSE, "\\u0BE7,\\u0BE8\\u0BE9\\u0BEA.\\u0BEB\\u0BEC\\u0BED" },
        { "ta_IN@numbers=traditional", 1235.0, TRUE, "\\u0BF2\\u0BE8\\u0BF1\\u0BE9\\u0BF0\\u0BEB" },
        { "ta_IN@numbers=finance", 1234.567, FALSE, "1,234.567" }, // fall back to default per TR35
        { "zh_TW@numbers=native", 1234.567, FALSE, "\\u4e00,\\u4e8c\\u4e09\\u56db.\\u4e94\\u516d\\u4e03" },
        { "zh_TW@numbers=traditional", 1234.567, TRUE, "\\u4E00\\u5343\\u4E8C\\u767E\\u4E09\\u5341\\u56DB\\u9EDE\\u4E94\\u516D\\u4E03" },
        { "zh_TW@numbers=finance", 1234.567, TRUE, "\\u58F9\\u4EDF\\u8CB3\\u4F70\\u53C3\\u62FE\\u8086\\u9EDE\\u4F0D\\u9678\\u67D2" },
        { NULL, 0, FALSE, NULL }
    };

    UErrorCode ec;

    const TestNumberingSystemItem *item;
    for (item = DATA; item->localeName != NULL; item++) {
        ec = U_ZERO_ERROR;
        Locale loc = Locale::createFromName(item->localeName);

        NumberFormat *origFmt = NumberFormat::createInstance(loc,ec);
        if (U_FAILURE(ec)) {
            dataerrln("FAIL: getInstance(%s) - %s", item->localeName, u_errorName(ec));
            continue;
        }
        // Clone to test ticket #10682
        NumberFormat *fmt = (NumberFormat *) origFmt->clone();
        delete origFmt;


        if (item->isRBNF) {
            expect3(*fmt,item->value,CharsToUnicodeString(item->expectedResult));
        } else {
            expect2(*fmt,item->value,CharsToUnicodeString(item->expectedResult));
        }
        delete fmt;
    }


    // Test bogus keyword value
    ec = U_ZERO_ERROR;
    Locale loc4 = Locale::createFromName("en_US@numbers=foobar");
    NumberFormat* fmt4= NumberFormat::createInstance(loc4, ec);
    if ( ec != U_UNSUPPORTED_ERROR ) {
        errln("FAIL: getInstance(en_US@numbers=foobar) should have returned U_UNSUPPORTED_ERROR");
        delete fmt4;
    }

    ec = U_ZERO_ERROR;
    NumberingSystem *ns = NumberingSystem::createInstance(ec);
    if (U_FAILURE(ec)) {
        dataerrln("FAIL: NumberingSystem::createInstance(ec); - %s", u_errorName(ec));
    }

    if ( ns != NULL ) {
        ns->getDynamicClassID();
        ns->getStaticClassID();
    } else {
        errln("FAIL: getInstance() returned NULL.");
    }

    NumberingSystem *ns1 = new NumberingSystem(*ns);
    if (ns1 == NULL) {
        errln("FAIL: NumberSystem copy constructor returned NULL.");
    }

    delete ns1;
    delete ns;

}