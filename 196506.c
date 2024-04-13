        void testParseUrlSpecialChar()
        {
            cxxtools::QueryParams q;
            q.parse_url("p1=value+with%20spaces&m%a4kitalo=tommi+");
            CXXTOOLS_UNIT_ASSERT(q.has("p1"));
            CXXTOOLS_UNIT_ASSERT(q.has("m\xa4kitalo"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["p1"], "value with spaces");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["m\xa4kitalo"], "tommi ");
        }