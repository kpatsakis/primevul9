void NumberFormatTest::expectPosition(FieldPosition& pos, int32_t id, int32_t start, int32_t limit,
                                       const UnicodeString& str)  {
  logln(str);
  assertTrue((UnicodeString)"id " + id + " == " + pos.getField(), id == pos.getField());
  assertTrue((UnicodeString)"begin " + start + " == " + pos.getBeginIndex(), start == pos.getBeginIndex());
  assertTrue((UnicodeString)"end " + limit + " == " + pos.getEndIndex(), limit == pos.getEndIndex());
}