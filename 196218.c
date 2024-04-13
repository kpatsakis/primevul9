void NumberFormatTest::TestDecimal() {
    {
        UErrorCode  status = U_ZERO_ERROR;
        Formattable f("12.345678999987654321E666", status);
        ASSERT_SUCCESS(status);
        StringPiece s = f.getDecimalNumber(status);
        ASSERT_SUCCESS(status);
        ASSERT_EQUALS("1.2345678999987654321E+667", s.data());
        //printf("%s\n", s.data());
    }

    {
        UErrorCode status = U_ZERO_ERROR;
        Formattable f1("this is not a number", status);
        ASSERT_EQUALS(U_DECIMAL_NUMBER_SYNTAX_ERROR, status);
    }

    {
        UErrorCode status = U_ZERO_ERROR;
        Formattable f;
        f.setDecimalNumber("123.45", status);
        ASSERT_SUCCESS(status);
        ASSERT_EQUALS( Formattable::kDouble, f.getType());
        ASSERT_EQUALS(123.45, f.getDouble());
        ASSERT_EQUALS(123.45, f.getDouble(status));
        ASSERT_SUCCESS(status);
        ASSERT_EQUALS("123.45", f.getDecimalNumber(status).data());
        ASSERT_SUCCESS(status);

        f.setDecimalNumber("4.5678E7", status);
        int32_t n;
        n = f.getLong();
        ASSERT_EQUALS(45678000, n);

        status = U_ZERO_ERROR;
        f.setDecimalNumber("-123", status);
        ASSERT_SUCCESS(status);
        ASSERT_EQUALS( Formattable::kLong, f.getType());
        ASSERT_EQUALS(-123, f.getLong());
        ASSERT_EQUALS(-123, f.getLong(status));
        ASSERT_SUCCESS(status);
        ASSERT_EQUALS("-123", f.getDecimalNumber(status).data());
        ASSERT_SUCCESS(status);

        status = U_ZERO_ERROR;
        f.setDecimalNumber("1234567890123", status);  // Number too big for 32 bits
        ASSERT_SUCCESS(status);
        ASSERT_EQUALS( Formattable::kInt64, f.getType());
        ASSERT_EQUALS(1234567890123LL, f.getInt64());
        ASSERT_EQUALS(1234567890123LL, f.getInt64(status));
        ASSERT_SUCCESS(status);
        ASSERT_EQUALS("1.234567890123E+12", f.getDecimalNumber(status).data());
        ASSERT_SUCCESS(status);
    }

    {
        UErrorCode status = U_ZERO_ERROR;
        NumberFormat *fmtr = NumberFormat::createInstance(Locale::getUS(), UNUM_DECIMAL, status);
        if (U_FAILURE(status) || fmtr == NULL) {
            dataerrln("Unable to create NumberFormat");
        } else {
            UnicodeString formattedResult;
            StringPiece num("244444444444444444444444444444444444446.4");
            fmtr->format(num, formattedResult, NULL, status);
            ASSERT_SUCCESS(status);
            ASSERT_EQUALS("244,444,444,444,444,444,444,444,444,444,444,444,446.4", formattedResult);
            //std::string ss; std::cout << formattedResult.toUTF8String(ss);
            delete fmtr;
        }
    }

    {
        // Check formatting a DigitList.  DigitList is internal, but this is
        // a critical interface that must work.
        UErrorCode status = U_ZERO_ERROR;
        NumberFormat *fmtr = NumberFormat::createInstance(Locale::getUS(), UNUM_DECIMAL, status);
        if (U_FAILURE(status) || fmtr == NULL) {
            dataerrln("Unable to create NumberFormat");
        } else {
            UnicodeString formattedResult;
            DecimalQuantity dl;
            StringPiece num("123.4566666666666666666666666666666666621E+40");
            dl.setToDecNumber(num, status);
            ASSERT_SUCCESS(status);
            fmtr->format(dl, formattedResult, NULL, status);
            ASSERT_SUCCESS(status);
            ASSERT_EQUALS("1,234,566,666,666,666,666,666,666,666,666,666,666,621,000", formattedResult);

            status = U_ZERO_ERROR;
            num.set("666.666");
            dl.setToDecNumber(num, status);
            FieldPosition pos(NumberFormat::FRACTION_FIELD);
            ASSERT_SUCCESS(status);
            formattedResult.remove();
            fmtr->format(dl, formattedResult, pos, status);
            ASSERT_SUCCESS(status);
            ASSERT_EQUALS("666.666", formattedResult);
            ASSERT_EQUALS(4, pos.getBeginIndex());
            ASSERT_EQUALS(7, pos.getEndIndex());
            delete fmtr;
        }
    }

    {
        // Check a parse with a formatter with a multiplier.
        UErrorCode status = U_ZERO_ERROR;
        NumberFormat *fmtr = NumberFormat::createInstance(Locale::getUS(), UNUM_PERCENT, status);
        if (U_FAILURE(status) || fmtr == NULL) {
            dataerrln("Unable to create NumberFormat");
        } else {
            UnicodeString input = "1.84%";
            Formattable result;
            fmtr->parse(input, result, status);
            ASSERT_SUCCESS(status);
            ASSERT_EQUALS("0.0184", result.getDecimalNumber(status).data());
            //std::cout << result.getDecimalNumber(status).data();
            delete fmtr;
        }
    }

#if U_PLATFORM != U_PF_CYGWIN || defined(CYGWINMSVC)
    /*
     * This test fails on Cygwin (1.7.16) using GCC because of a rounding issue with strtod().
     * See #9463
     */
    {
        // Check that a parse returns a decimal number with full accuracy
        UErrorCode status = U_ZERO_ERROR;
        NumberFormat *fmtr = NumberFormat::createInstance(Locale::getUS(), UNUM_DECIMAL, status);
        if (U_FAILURE(status) || fmtr == NULL) {
            dataerrln("Unable to create NumberFormat");
        } else {
            UnicodeString input = "1.002200044400088880000070000";
            Formattable result;
            fmtr->parse(input, result, status);
            ASSERT_SUCCESS(status);
            ASSERT_EQUALS(0, strcmp("1.00220004440008888000007", result.getDecimalNumber(status).data()));
            ASSERT_EQUALS(1.00220004440008888,   result.getDouble());
            //std::cout << result.getDecimalNumber(status).data();
            delete fmtr;
        }
    }
#endif

}