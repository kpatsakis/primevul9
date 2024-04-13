void NumberFormatTest::TestCases() {
    UErrorCode ec = U_ZERO_ERROR;
    TextFile reader("NumberFormatTestCases.txt", "UTF8", ec);
    if (U_FAILURE(ec)) {
        dataerrln("Couldn't open NumberFormatTestCases.txt");
        return;
    }
    TokenIterator tokens(&reader);

    Locale loc("en", "US", "");
    DecimalFormat *ref = 0, *fmt = 0;
    MeasureFormat *mfmt = 0;
    UnicodeString pat, tok, mloc, str, out, where, currAmt;
    Formattable n;

    for (;;) {
        ec = U_ZERO_ERROR;
        if (!tokens.next(tok, ec)) {
            break;
        }
        where = UnicodeString("(") + tokens.getLineNumber() + ") ";
        int32_t cmd = keywordIndex(tok);
        switch (cmd) {
        case 0:
            // ref= <reference pattern>
            if (!tokens.next(tok, ec)) goto error;
            delete ref;
            ref = new DecimalFormat(tok,
                      new DecimalFormatSymbols(Locale::getUS(), ec), ec);
            if (U_FAILURE(ec)) {
                dataerrln("Error constructing DecimalFormat");
                goto error;
            }
            break;
        case 1:
            // loc= <locale>
            if (!tokens.next(tok, ec)) goto error;
            loc = Locale::createFromName(CharString().appendInvariantChars(tok, ec).data());
            break;
        case 2: // f:
        case 3: // fp:
        case 4: // rt:
        case 5: // p:
            if (!tokens.next(tok, ec)) goto error;
            if (tok != "-") {
                pat = tok;
                delete fmt;
                fmt = new DecimalFormat(pat, new DecimalFormatSymbols(loc, ec), ec);
                if (U_FAILURE(ec)) {
                    errln("FAIL: " + where + "Pattern \"" + pat + "\": " + u_errorName(ec));
                    ec = U_ZERO_ERROR;
                    if (!tokens.next(tok, ec)) goto error;
                    if (!tokens.next(tok, ec)) goto error;
                    if (cmd == 3) {
                        if (!tokens.next(tok, ec)) goto error;
                    }
                    continue;
                }
            }
            if (cmd == 2 || cmd == 3 || cmd == 4) {
                // f: <pattern or '-'> <number> <exp. string>
                // fp: <pattern or '-'> <number> <exp. string> <exp. number>
                // rt: <pattern or '-'> <number> <string>
                UnicodeString num;
                if (!tokens.next(num, ec)) goto error;
                if (!tokens.next(str, ec)) goto error;
                ref->parse(num, n, ec);
                assertSuccess("parse", ec);
                assertEquals(where + "\"" + pat + "\".format(" + num + ")",
                             str, fmt->format(n, out.remove(), ec));
                assertSuccess("format", ec);
                if (cmd == 3) { // fp:
                    if (!tokens.next(num, ec)) goto error;
                    ref->parse(num, n, ec);
                    assertSuccess("parse", ec);
                }
                if (cmd != 2) { // != f:
                    Formattable m;
                    fmt->parse(str, m, ec);
                    assertSuccess("parse", ec);
                    assertEquals(where + "\"" + pat + "\".parse(\"" + str + "\")",
                                 n, m);
                }
            }
            // p: <pattern or '-'> <string to parse> <exp. number>
            else {
                UnicodeString expstr;
                if (!tokens.next(str, ec)) goto error;
                if (!tokens.next(expstr, ec)) goto error;
                Formattable exp, n;
                ref->parse(expstr, exp, ec);
                assertSuccess("parse", ec);
                fmt->parse(str, n, ec);
                assertSuccess("parse", ec);
                assertEquals(where + "\"" + pat + "\".parse(\"" + str + "\")",
                             exp, n);
            }
            break;
        case 8: // fpc:
            if (!tokens.next(tok, ec)) goto error;
            if (tok != "-") {
                mloc = tok;
                delete mfmt;
                mfmt = MeasureFormat::createCurrencyFormat(
                    Locale::createFromName(
                        CharString().appendInvariantChars(mloc, ec).data()), ec);
                if (U_FAILURE(ec)) {
                    errln("FAIL: " + where + "Loc \"" + mloc + "\": " + u_errorName(ec));
                    ec = U_ZERO_ERROR;
                    if (!tokens.next(tok, ec)) goto error;
                    if (!tokens.next(tok, ec)) goto error;
                    if (!tokens.next(tok, ec)) goto error;
                    continue;
                }
            } else if (mfmt == NULL) {
                errln("FAIL: " + where + "Loc \"" + mloc + "\": skip case using previous locale, no valid MeasureFormat");
                if (!tokens.next(tok, ec)) goto error;
                if (!tokens.next(tok, ec)) goto error;
                if (!tokens.next(tok, ec)) goto error;
                continue;
            }
            // fpc: <loc or '-'> <curr.amt> <exp. string> <exp. curr.amt>
            if (!tokens.next(currAmt, ec)) goto error;
            if (!tokens.next(str, ec)) goto error;
            parseCurrencyAmount(currAmt, *ref, (UChar)0x2F/*'/'*/, n, ec);
            if (assertSuccess("parseCurrencyAmount", ec)) {
                assertEquals(where + "getCurrencyFormat(" + mloc + ").format(" + currAmt + ")",
                             str, mfmt->format(n, out.remove(), ec));
                assertSuccess("format", ec);
            }
            if (!tokens.next(currAmt, ec)) goto error;
            parseCurrencyAmount(currAmt, *ref, (UChar)0x2F/*'/'*/, n, ec);
            if (assertSuccess("parseCurrencyAmount", ec)) {
                Formattable m;

                mfmt->parseObject(str, m, ec);
                if (assertSuccess("parseCurrency", ec)) {
                    assertEquals(where + "getCurrencyFormat(" + mloc + ").parse(\"" + str + "\")",
                                 n, m);
                } else {
                    errln("FAIL: source " + str);
                }
            }
            break;
        case 6:
            // perr: <pattern or '-'> <invalid string>
            errln("FAIL: Under construction");
            goto done;
        case 7: {
            // pat: <pattern> <exp. toPattern, or '-' or 'err'>
            UnicodeString testpat;
            UnicodeString exppat;
            if (!tokens.next(testpat, ec)) goto error;
            if (!tokens.next(exppat, ec)) goto error;
            UBool err = exppat == "err";
            UBool existingPat = FALSE;
            if (testpat == "-") {
                if (err) {
                    errln("FAIL: " + where + "Invalid command \"pat: - err\"");
                    continue;
                }
                existingPat = TRUE;
                testpat = pat;
            }
            if (exppat == "-") exppat = testpat;
            DecimalFormat* f = 0;
            UErrorCode ec2 = U_ZERO_ERROR;
            if (existingPat) {
                f = fmt;
            } else {
                f = new DecimalFormat(testpat, ec2);
            }
            if (U_SUCCESS(ec2)) {
                if (err) {
                    errln("FAIL: " + where + "Invalid pattern \"" + testpat +
                          "\" was accepted");
                } else {
                    UnicodeString pat2;
                    assertEquals(where + "\"" + testpat + "\".toPattern()",
                                 exppat, f->toPattern(pat2));
                }
            } else {
                if (err) {
                    logln("Ok: " + where + "Invalid pattern \"" + testpat +
                          "\" failed: " + u_errorName(ec2));
                } else {
                    errln("FAIL: " + where + "Valid pattern \"" + testpat +
                          "\" failed: " + u_errorName(ec2));
                }
            }
            if (!existingPat) delete f;
            } break;
        case -1:
            errln("FAIL: " + where + "Unknown command \"" + tok + "\"");
            goto done;
        }
    }
    goto done;

 error:
    if (U_SUCCESS(ec)) {
        errln("FAIL: Unexpected EOF");
    } else {
        errcheckln(ec, "FAIL: " + where + "Unexpected " + u_errorName(ec));
    }

 done:
    delete mfmt;
    delete fmt;
    delete ref;
}