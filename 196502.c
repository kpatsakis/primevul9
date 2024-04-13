        void testIterator()
        {
            cxxtools::QueryParams q;
            q.add("p1", "value1");
            q.add("p2", "value2");
            q.add("p2", "value3");
            q.add("value4");
            q.add("value5");

            cxxtools::QueryParams::const_iterator it;

            it = q.begin();
            CXXTOOLS_UNIT_ASSERT(it != q.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(*it, "value4");
            ++it;
            CXXTOOLS_UNIT_ASSERT(it != q.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(*it, "value5");
            ++it;
            CXXTOOLS_UNIT_ASSERT(it == q.end());

            it = q.begin("p1");
            CXXTOOLS_UNIT_ASSERT(it != q.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(*it, "value1");
            ++it;
            CXXTOOLS_UNIT_ASSERT(it == q.end());

            it = q.begin("p2");
            CXXTOOLS_UNIT_ASSERT(it != q.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(*it, "value2");
            ++it;
            CXXTOOLS_UNIT_ASSERT(it != q.end());
            CXXTOOLS_UNIT_ASSERT_EQUALS(*it, "value3");
            ++it;
            CXXTOOLS_UNIT_ASSERT(it == q.end());
        }