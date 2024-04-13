NumberFormatTest::TestLenientParse(void)
{
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat *format = new DecimalFormat("(#,##0)", status);
    Formattable n;

    if (format == NULL || U_FAILURE(status)) {
        dataerrln("Unable to create DecimalFormat (#,##0) - %s", u_errorName(status));
    } else {
        format->setLenient(TRUE);
        for (int32_t t = 0; t < UPRV_LENGTHOF (lenientAffixTestCases); t += 1) {
        	UnicodeString testCase = ctou(lenientAffixTestCases[t]);

            format->parse(testCase, n, status);
            logln((UnicodeString)"parse(" + testCase + ") = " + n.getLong());

            if (U_FAILURE(status) || n.getType() != Formattable::kLong ||
            	n.getLong() != 1) {
            	dataerrln((UnicodeString)"Lenient parse failed for \"" + (UnicodeString) lenientAffixTestCases[t]
                      + (UnicodeString) "\"; error code = " + u_errorName(status));
            	status = U_ZERO_ERROR;
            }
       }
       delete format;
    }

    Locale en_US("en_US");
    Locale sv_SE("sv_SE");

    NumberFormat *mFormat = NumberFormat::createInstance(sv_SE, UNUM_DECIMAL, status);

    if (mFormat == NULL || U_FAILURE(status)) {
        dataerrln("Unable to create NumberFormat (sv_SE, UNUM_DECIMAL) - %s", u_errorName(status));
    } else {
        mFormat->setLenient(TRUE);
        for (int32_t t = 0; t < UPRV_LENGTHOF(lenientMinusTestCases); t += 1) {
            UnicodeString testCase = ctou(lenientMinusTestCases[t]);

            mFormat->parse(testCase, n, status);
            logln((UnicodeString)"parse(" + testCase + ") = " + n.getLong());

            if (U_FAILURE(status) || n.getType() != Formattable::kLong || n.getLong() != -5) {
                errln((UnicodeString)"Lenient parse failed for \"" + (UnicodeString) lenientMinusTestCases[t]
                      + (UnicodeString) "\"; error code = " + u_errorName(status));
                status = U_ZERO_ERROR;
            }
        }
        delete mFormat;
    }

    mFormat = NumberFormat::createInstance(en_US, UNUM_DECIMAL, status);

    if (mFormat == NULL || U_FAILURE(status)) {
        dataerrln("Unable to create NumberFormat (en_US, UNUM_DECIMAL) - %s", u_errorName(status));
    } else {
        mFormat->setLenient(TRUE);
        for (int32_t t = 0; t < UPRV_LENGTHOF(lenientMinusTestCases); t += 1) {
            UnicodeString testCase = ctou(lenientMinusTestCases[t]);

            mFormat->parse(testCase, n, status);
            logln((UnicodeString)"parse(" + testCase + ") = " + n.getLong());

            if (U_FAILURE(status) || n.getType() != Formattable::kLong || n.getLong() != -5) {
                errln((UnicodeString)"Lenient parse failed for \"" + (UnicodeString) lenientMinusTestCases[t]
                      + (UnicodeString) "\"; error code = " + u_errorName(status));
                status = U_ZERO_ERROR;
            }
        }
        delete mFormat;
    }

    NumberFormat *cFormat = NumberFormat::createInstance(en_US, UNUM_CURRENCY, status);

    if (cFormat == NULL || U_FAILURE(status)) {
        dataerrln("Unable to create NumberFormat (en_US, UNUM_CURRENCY) - %s", u_errorName(status));
    } else {
        cFormat->setLenient(TRUE);
        for (int32_t t = 0; t < UPRV_LENGTHOF (lenientCurrencyTestCases); t += 1) {
        	UnicodeString testCase = ctou(lenientCurrencyTestCases[t]);

            cFormat->parse(testCase, n, status);
            logln((UnicodeString)"parse(" + testCase + ") = " + n.getLong());

            if (U_FAILURE(status) ||n.getType() != Formattable::kLong ||
            	n.getLong() != 1000) {
            	errln((UnicodeString)"Lenient parse failed for \"" + (UnicodeString) lenientCurrencyTestCases[t]
                      + (UnicodeString) "\"; error code = " + u_errorName(status));
            	status = U_ZERO_ERROR;
            }
        }

        for (int32_t t = 0; t < UPRV_LENGTHOF (lenientNegativeCurrencyTestCases); t += 1) {
        	UnicodeString testCase = ctou(lenientNegativeCurrencyTestCases[t]);

            cFormat->parse(testCase, n, status);
            logln((UnicodeString)"parse(" + testCase + ") = " + n.getLong());

            if (U_FAILURE(status) ||n.getType() != Formattable::kLong ||
            	n.getLong() != -1000) {
            	errln((UnicodeString)"Lenient parse failed for \"" + (UnicodeString) lenientNegativeCurrencyTestCases[t]
                      + (UnicodeString) "\"; error code = " + u_errorName(status));
            	status = U_ZERO_ERROR;
            }
        }

        delete cFormat;
    }

    NumberFormat *pFormat = NumberFormat::createPercentInstance(en_US, status);

    if (pFormat == NULL || U_FAILURE(status)) {
        dataerrln("Unable to create NumberFormat::createPercentInstance (en_US) - %s", u_errorName(status));
    } else {
        pFormat->setLenient(TRUE);
        for (int32_t t = 0; t < UPRV_LENGTHOF (lenientPercentTestCases); t += 1) {
        	UnicodeString testCase = ctou(lenientPercentTestCases[t]);

        	pFormat->parse(testCase, n, status);
            logln((UnicodeString)"parse(" + testCase + ") = " + n.getDouble());

            if (U_FAILURE(status) ||n.getType() != Formattable::kDouble ||
            	n.getDouble() != 0.25) {
            	errln((UnicodeString)"Lenient parse failed for \"" + (UnicodeString) lenientPercentTestCases[t]
                      + (UnicodeString) "\"; error code = " + u_errorName(status)
                      + "; got: " + n.getDouble(status));
            	status = U_ZERO_ERROR;
            }
        }

        for (int32_t t = 0; t < UPRV_LENGTHOF (lenientNegativePercentTestCases); t += 1) {
        	UnicodeString testCase = ctou(lenientNegativePercentTestCases[t]);

        	pFormat->parse(testCase, n, status);
            logln((UnicodeString)"parse(" + testCase + ") = " + n.getDouble());

            if (U_FAILURE(status) ||n.getType() != Formattable::kDouble ||
            	n.getDouble() != -0.25) {
            	errln((UnicodeString)"Lenient parse failed for \"" + (UnicodeString) lenientNegativePercentTestCases[t]
                      + (UnicodeString) "\"; error code = " + u_errorName(status)
                      + "; got: " + n.getDouble(status));
            	status = U_ZERO_ERROR;
            }
        }

        delete pFormat;
    }

   // Test cases that should fail with a strict parse and pass with a
   // lenient parse.
   NumberFormat *nFormat = NumberFormat::createInstance(en_US, status);

   if (nFormat == NULL || U_FAILURE(status)) {
       dataerrln("Unable to create NumberFormat (en_US) - %s", u_errorName(status));
   } else {
       // first, make sure that they fail with a strict parse
       for (int32_t t = 0; t < UPRV_LENGTHOF(strictFailureTestCases); t += 1) {
	       UnicodeString testCase = ctou(strictFailureTestCases[t]);

	       nFormat->parse(testCase, n, status);
	       logln((UnicodeString)"parse(" + testCase + ") = " + n.getLong());

	       if (! U_FAILURE(status)) {
		       errln((UnicodeString)"Strict Parse succeeded for \"" + (UnicodeString) strictFailureTestCases[t]
                     + (UnicodeString) "\"; error code = " + u_errorName(status));
	       }

	       status = U_ZERO_ERROR;
       }

       // then, make sure that they pass with a lenient parse
       nFormat->setLenient(TRUE);
       for (int32_t t = 0; t < UPRV_LENGTHOF(strictFailureTestCases); t += 1) {
	       UnicodeString testCase = ctou(strictFailureTestCases[t]);

	       nFormat->parse(testCase, n, status);
	       logln((UnicodeString)"parse(" + testCase + ") = " + n.getLong());

	       if (U_FAILURE(status) ||n.getType() != Formattable::kLong ||
	            	n.getLong() != 1000) {
		       errln((UnicodeString)"Lenient parse failed for \"" + (UnicodeString) strictFailureTestCases[t]
                     + (UnicodeString) "\"; error code = " + u_errorName(status));
		       status = U_ZERO_ERROR;
	       }
       }

       delete nFormat;
   }
}