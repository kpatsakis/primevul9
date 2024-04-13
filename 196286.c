void NumberFormatTest::expectPositions(FieldPositionIterator& iter, int32_t *values, int32_t tupleCount,
                                       const UnicodeString& str)  {
  UBool found[10];
  FieldPosition fp;

  if (tupleCount > 10) {
    assertTrue("internal error, tupleCount too large", FALSE);
  } else {
    for (int i = 0; i < tupleCount; ++i) {
      found[i] = FALSE;
    }
  }

  logln(str);
  while (iter.next(fp)) {
    UBool ok = FALSE;
    int32_t id = fp.getField();
    int32_t start = fp.getBeginIndex();
    int32_t limit = fp.getEndIndex();

    // is there a logln using printf?
    char buf[128];
    sprintf(buf, "%24s %3d %3d %3d", attrString(id), id, start, limit);
    logln(buf);

    for (int i = 0; i < tupleCount; ++i) {
      if (found[i]) {
        continue;
      }
      if (values[i*3] == id &&
          values[i*3+1] == start &&
          values[i*3+2] == limit) {
        found[i] = ok = TRUE;
        break;
      }
    }

    assertTrue((UnicodeString)"found [" + id + "," + start + "," + limit + "]", ok);
  }

  // check that all were found
  UBool ok = TRUE;
  for (int i = 0; i < tupleCount; ++i) {
    if (!found[i]) {
      ok = FALSE;
      assertTrue((UnicodeString) "missing [" + values[i*3] + "," + values[i*3+1] + "," + values[i*3+2] + "]", found[i]);
    }
  }
  assertTrue("no expected values were missing", ok);
}