void NumberFormatTest::Test13763_FieldPositionIteratorOffset() {
    IcuTestErrorCode status(*this, "Test13763_FieldPositionIteratorOffset");
    FieldPositionIterator fpi;
    UnicodeString result(u"foo\U0001F4FBbar"); // 8 code units
    LocalPointer<NumberFormat> nf(NumberFormat::createInstance("en", status), status);
    if (!assertSuccess("", status, true, __FILE__, __LINE__)) { return; }
    nf->format(5142.3, result, &fpi, status);

    int32_t expected[] = {
      UNUM_GROUPING_SEPARATOR_FIELD, 9, 10,
      UNUM_INTEGER_FIELD, 8, 13,
      UNUM_DECIMAL_SEPARATOR_FIELD, 13, 14,
      UNUM_FRACTION_FIELD, 14, 15,
    };
    int32_t tupleCount = UPRV_LENGTHOF(expected)/3;
    expectPositions(fpi, expected, tupleCount, result);
}