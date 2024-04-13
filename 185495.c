TEST(TestHeaderMapImplDeathTest, TestHeaderLengthChecks) {
  HeaderString value;
  value.setCopy("some;");
  EXPECT_DEATH(value.append(nullptr, std::numeric_limits<uint32_t>::max()),
               "Trying to allocate overly large headers.");

  std::string source("hello");
  HeaderString reference;
  reference.setReference(source);
  EXPECT_DEATH(reference.append(nullptr, std::numeric_limits<uint32_t>::max()),
               "Trying to allocate overly large headers.");
}