        void testCombine()
        {
            cxxtools::QueryParams q;
            cxxtools::QueryParams q2;
            q.add("p1", "value1");
            q2.add("p2", "value2");
            q2.add("value3");
            q.add(q2);
        }