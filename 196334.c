void NumberFormatTest::TestFormatAttributes() {
  Locale locale("en_US");
  UErrorCode status = U_ZERO_ERROR;
  DecimalFormat *decFmt = (DecimalFormat *) NumberFormat::createInstance(locale, UNUM_CURRENCY, status);
    if (failure(status, "NumberFormat::createInstance", TRUE)) return;
  double val = 12345.67;

  {
    int32_t expected[] = {
      UNUM_CURRENCY_FIELD, 0, 1,
      UNUM_GROUPING_SEPARATOR_FIELD, 3, 4,
      UNUM_INTEGER_FIELD, 1, 7,
      UNUM_DECIMAL_SEPARATOR_FIELD, 7, 8,
      UNUM_FRACTION_FIELD, 8, 10,
    };
    int32_t tupleCount = UPRV_LENGTHOF(expected)/3;

    FieldPositionIterator posIter;
    UnicodeString result;
    decFmt->format(val, result, &posIter, status);
    expectPositions(posIter, expected, tupleCount, result);
  }
  {
    FieldPosition fp(UNUM_INTEGER_FIELD);
    UnicodeString result;
    decFmt->format(val, result, fp);
    expectPosition(fp, UNUM_INTEGER_FIELD, 1, 7, result);
  }
  {
    FieldPosition fp(UNUM_FRACTION_FIELD);
    UnicodeString result;
    decFmt->format(val, result, fp);
    expectPosition(fp, UNUM_FRACTION_FIELD, 8, 10, result);
  }
  delete decFmt;

  decFmt = (DecimalFormat *) NumberFormat::createInstance(locale, UNUM_SCIENTIFIC, status);
  val = -0.0000123;
  {
    int32_t expected[] = {
      UNUM_SIGN_FIELD, 0, 1,
      UNUM_INTEGER_FIELD, 1, 2,
      UNUM_DECIMAL_SEPARATOR_FIELD, 2, 3,
      UNUM_FRACTION_FIELD, 3, 5,
      UNUM_EXPONENT_SYMBOL_FIELD, 5, 6,
      UNUM_EXPONENT_SIGN_FIELD, 6, 7,
      UNUM_EXPONENT_FIELD, 7, 8
    };
    int32_t tupleCount = UPRV_LENGTHOF(expected)/3;

    FieldPositionIterator posIter;
    UnicodeString result;
    decFmt->format(val, result, &posIter, status);
    expectPositions(posIter, expected, tupleCount, result);
  }
  {
    FieldPosition fp(UNUM_INTEGER_FIELD);
    UnicodeString result;
    decFmt->format(val, result, fp);
    expectPosition(fp, UNUM_INTEGER_FIELD, 1, 2, result);
  }
  {
    FieldPosition fp(UNUM_FRACTION_FIELD);
    UnicodeString result;
    decFmt->format(val, result, fp);
    expectPosition(fp, UNUM_FRACTION_FIELD, 3, 5, result);
  }
  delete decFmt;

  fflush(stderr);
}