        void testParseDoublePercent()
        {
            cxxtools::QueryParams q;
            q.parse_url("%%=%%%");
            CXXTOOLS_UNIT_ASSERT(q.has("%%"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["%%"], "%%%");
        }