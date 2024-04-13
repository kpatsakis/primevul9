        QueryParamsTest()
            : cxxtools::unit::TestSuite("queryparams")
        {
            registerMethod("testQueryParams", *this, &QueryParamsTest::testQueryParams);
            registerMethod("testCopy", *this, &QueryParamsTest::testCopy);
            registerMethod("testParseUrl", *this, &QueryParamsTest::testParseUrl);
            registerMethod("testParseUrlSpecialChar", *this, &QueryParamsTest::testParseUrlSpecialChar);
            registerMethod("testParseDoublePercent", *this, &QueryParamsTest::testParseDoublePercent);
            registerMethod("testCount", *this, &QueryParamsTest::testCount);
            registerMethod("testCombine", *this, &QueryParamsTest::testCombine);
            registerMethod("testIterator", *this, &QueryParamsTest::testIterator);
            registerMethod("testGetUrl", *this, &QueryParamsTest::testGetUrl);
        }