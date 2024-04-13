        void testCount()
        {
            cxxtools::QueryParams q;
            q.add("p1", "value1");
            q.add("p2", "value2");
            q.add("p2", "value3");
            q.add("value4");
            q.add("value5");
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.paramcount(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.paramcount("p1"), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(q.paramcount("p2"), 2);
        }