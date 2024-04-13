        void testGetUrl()
        {
            cxxtools::QueryParams q;
            q.add("first name", "Tommi");
            q.add("last name", "M\xa4kitalo");
            q.add("some value");

            std::string url = q.getUrl();
            CXXTOOLS_UNIT_ASSERT_EQUALS(url, "first+name=Tommi&last+name=M%A4kitalo&some+value");
        }