        void testQueryParams()
        {
            cxxtools::QueryParams q;
            q.add("p1", "value1");
            q.add("p2", "value2");
            q.add("value3");
            CXXTOOLS_UNIT_ASSERT(q.has("p1"));
            CXXTOOLS_UNIT_ASSERT(q.has("p2"));
            CXXTOOLS_UNIT_ASSERT(!q.has("p3"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["p1"], "value1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q["p2"], "value2");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q[0], "value3");
        }