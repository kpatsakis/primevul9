UBool NumberFormatTest::testFormattableAsUFormattable(const char *file, int line, Formattable &f) {
  UnicodeString fileLine = UnicodeString(file)+UnicodeString(":")+line+UnicodeString(": ");

  UFormattable *u = f.toUFormattable();
  logln();
  if (u == NULL) {
    errln("%s:%d: Error: f.toUFormattable() retuned NULL.");
    return FALSE;
  }
  logln("%s:%d: comparing Formattable with UFormattable", file, line);
  logln(fileLine + toString(f));

  UErrorCode status = U_ZERO_ERROR;
  UErrorCode valueStatus = U_ZERO_ERROR;
  UFormattableType expectUType = UFMT_COUNT; // invalid

  UBool triedExact = FALSE; // did we attempt an exact comparison?
  UBool exactMatch = FALSE; // was the exact comparison true?

  switch( f.getType() ) {
  case Formattable::kDate:
    expectUType = UFMT_DATE;
    exactMatch = (f.getDate()==ufmt_getDate(u, &valueStatus));
    triedExact = TRUE;
    break;
  case Formattable::kDouble:
    expectUType = UFMT_DOUBLE;
    exactMatch = (f.getDouble()==ufmt_getDouble(u, &valueStatus));
    triedExact = TRUE;
    break;
  case Formattable::kLong:
    expectUType = UFMT_LONG;
    exactMatch = (f.getLong()==ufmt_getLong(u, &valueStatus));
    triedExact = TRUE;
    break;
  case Formattable::kString:
    expectUType = UFMT_STRING;
    {
      UnicodeString str;
      f.getString(str);
      int32_t len;
      const UChar* uch = ufmt_getUChars(u, &len, &valueStatus);
      if(U_SUCCESS(valueStatus)) {
        UnicodeString str2(uch, len);
        assertTrue("UChar* NULL-terminated", uch[len]==0);
        exactMatch = (str == str2);
      }
      triedExact = TRUE;
    }
    break;
  case Formattable::kArray:
    expectUType = UFMT_ARRAY;
    triedExact = TRUE;
    {
      int32_t count = ufmt_getArrayLength(u, &valueStatus);
      int32_t count2;
      const Formattable *array2 = f.getArray(count2);
      exactMatch = assertEquals(fileLine + " array count", count, count2);

      if(exactMatch) {
        for(int i=0;U_SUCCESS(valueStatus) && i<count;i++) {
          UFormattable *uu = ufmt_getArrayItemByIndex(u, i, &valueStatus);
          if(*Formattable::fromUFormattable(uu) != (array2[i])) {
            errln("%s:%d: operator== did not match at index[%d] - %p vs %p", file, line, i,
                  (const void*)Formattable::fromUFormattable(uu), (const void*)&(array2[i]));
            exactMatch = FALSE;
          } else {
            if(!testFormattableAsUFormattable("(sub item)",i,*Formattable::fromUFormattable(uu))) {
              exactMatch = FALSE;
            }
          }
        }
      }
    }
    break;
  case Formattable::kInt64:
    expectUType = UFMT_INT64;
    exactMatch = (f.getInt64()==ufmt_getInt64(u, &valueStatus));
    triedExact = TRUE;
    break;
  case Formattable::kObject:
    expectUType = UFMT_OBJECT;
    exactMatch = (f.getObject()==ufmt_getObject(u, &valueStatus));
    triedExact = TRUE;
    break;
  }
  UFormattableType uType = ufmt_getType(u, &status);

  if(U_FAILURE(status)) {
    errln("%s:%d: Error calling ufmt_getType - %s", file, line, u_errorName(status));
    return FALSE;
  }

  if(uType != expectUType) {
    errln("%s:%d: got type (%d) expected (%d) from ufmt_getType", file, line, (int) uType, (int) expectUType);
  }

  if(triedExact) {
    if(U_FAILURE(valueStatus)) {
      errln("%s:%d: got err %s trying to ufmt_get...() for exact match check", file, line, u_errorName(valueStatus));
    } else if(!exactMatch) {
     errln("%s:%d: failed exact match for the Formattable type", file, line);
    } else {
      logln("%s:%d: exact match OK", file, line);
    }
  } else {
    logln("%s:%d: note, did not attempt exact match for this formattable type", file, line);
  }

  if( assertEquals(fileLine + " isNumeric()", f.isNumeric(), ufmt_isNumeric(u))
      && f.isNumeric()) {
    UErrorCode convStatus = U_ZERO_ERROR;

    if(uType != UFMT_INT64) { // may fail to compare
      assertTrue(fileLine + " as doubles ==", f.getDouble(convStatus)==ufmt_getDouble(u, &convStatus));
    }

    if( assertSuccess(fileLine + " (numeric conversion status)", convStatus) ) {
      StringPiece fDecNum = f.getDecimalNumber(convStatus);
#if 1
      int32_t len;
      const char *decNumChars = ufmt_getDecNumChars(u, &len, &convStatus);
#else
      // copy version
      char decNumChars[200];
      int32_t len = ufmt_getDecNumChars(u, decNumChars, 200, &convStatus);
#endif

      if( assertSuccess(fileLine + " (decNumbers conversion)", convStatus) ) {
        logln(fileLine + decNumChars);
        assertEquals(fileLine + " decNumChars length==", len, fDecNum.length());
        assertEquals(fileLine + " decNumChars digits", decNumChars, fDecNum.data());
      }

      UErrorCode int64ConversionF = U_ZERO_ERROR;
      int64_t l = f.getInt64(int64ConversionF);
      UErrorCode int64ConversionU = U_ZERO_ERROR;
      int64_t r = ufmt_getInt64(u, &int64ConversionU);

      if( (l==r)
          && ( uType != UFMT_INT64 ) // int64 better not overflow
          && (U_INVALID_FORMAT_ERROR==int64ConversionU)
          && (U_INVALID_FORMAT_ERROR==int64ConversionF) ) {
        logln("%s:%d: OK: 64 bit overflow", file, line);
      } else {
        assertEquals(fileLine + " as int64 ==", l, r);
        assertSuccess(fileLine + " Formattable.getnt64()", int64ConversionF);
        assertSuccess(fileLine + " ufmt_getInt64()", int64ConversionU);
      }
    }
  }
  return exactMatch || !triedExact;
}