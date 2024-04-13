void NumberFormatTest::TestFieldPositionIterator() {
  // bug 7372
  UErrorCode status = U_ZERO_ERROR;
  FieldPositionIterator iter1;
  FieldPositionIterator iter2;
  FieldPosition pos;

  DecimalFormat *decFmt = (DecimalFormat *) NumberFormat::createInstance(status);
  if (failure(status, "NumberFormat::createInstance", TRUE)) return;

  double num = 1234.56;
  UnicodeString str1;
  UnicodeString str2;

  assertTrue((UnicodeString)"self==", iter1 == iter1);
  assertTrue((UnicodeString)"iter1==iter2", iter1 == iter2);

  decFmt->format(num, str1, &iter1, status);
  assertTrue((UnicodeString)"iter1 != iter2", iter1 != iter2);
  decFmt->format(num, str2, &iter2, status);
  assertTrue((UnicodeString)"iter1 == iter2 (2)", iter1 == iter2);
  iter1.next(pos);
  assertTrue((UnicodeString)"iter1 != iter2 (2)", iter1 != iter2);
  iter2.next(pos);
  assertTrue((UnicodeString)"iter1 == iter2 (3)", iter1 == iter2);

  // should format ok with no iterator
  str2.remove();
  decFmt->format(num, str2, NULL, status);
  assertEquals("null fpiter", str1, str2);

  delete decFmt;
}