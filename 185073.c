MATCHER_P(MapEqValue, rhs, "") {
  const ProtobufWkt::Struct& obj = arg;
  EXPECT_TRUE(!rhs.empty());
  for (auto const& entry : rhs) {
    EXPECT_TRUE(TestUtility::protoEqual(obj.fields().at(entry.first), entry.second));
  }
  return true;
}