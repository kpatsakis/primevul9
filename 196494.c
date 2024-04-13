        void testCopy()
        {
            cxxtools::QueryParams q;
            q.add("p1", "value1");
            q.add("p2", "value2");
            q.add("value3");
            cxxtools::QueryParams q2 = q;
            CXXTOOLS_UNIT_ASSERT(q2.has("p1"));
            CXXTOOLS_UNIT_ASSERT(q2.has("p2"));
            CXXTOOLS_UNIT_ASSERT(!q2.has("p3"));
            CXXTOOLS_UNIT_ASSERT_EQUALS(q2["p1"], "value1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q2["p2"], "value2");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q2[0], "value3");
        }