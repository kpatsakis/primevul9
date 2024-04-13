void NumberFormatTest::TestFormatFastpaths() {
    // get some additional case
    {
        UErrorCode status=U_ZERO_ERROR;
        DecimalFormat df(UnicodeString(u"0000"),status);
        if (U_FAILURE(status)) {
            dataerrln("Error creating DecimalFormat - %s", u_errorName(status));
        } else {
            int64_t long_number = 1;
            UnicodeString expect = "0001";
            UnicodeString result;
            FieldPosition pos;
            df.format(long_number, result, pos);
            if(U_FAILURE(status)||expect!=result) {
                dataerrln("%s:%d FAIL: expected '%s' got '%s' status %s",
                          __FILE__, __LINE__, CStr(expect)(), CStr(result)(), u_errorName(status));
             } else {
                logln("OK:  got expected '"+result+"' status "+UnicodeString(u_errorName(status),""));
            }
        }
    }
    {
        UErrorCode status=U_ZERO_ERROR;
        DecimalFormat df(UnicodeString(u"0000000000000000000"),status);
        if (U_FAILURE(status)) {
            dataerrln("Error creating DecimalFormat - %s", u_errorName(status));
        } else {
            int64_t long_number = U_INT64_MIN; // -9223372036854775808L;
            // uint8_t bits[8];
            // memcpy(bits,&long_number,8);
            // for(int i=0;i<8;i++) {
            //   logln("bits: %02X", (unsigned int)bits[i]);
            // }
            UnicodeString expect = "-9223372036854775808";
            UnicodeString result;
            FieldPosition pos;
            df.format(long_number, result, pos);
            if(U_FAILURE(status)||expect!=result) {
                dataerrln("%s:%d FAIL: expected '%s' got '%s' status %s on -9223372036854775808",
                          __FILE__, __LINE__, CStr(expect)(), CStr(result)(), u_errorName(status));
            } else {
                logln("OK:  got expected '"+result+"' status "+UnicodeString(u_errorName(status),"")+" on -9223372036854775808");
            }
        }
    }
    {
        UErrorCode status=U_ZERO_ERROR;
        DecimalFormat df(UnicodeString(u"0000000000000000000"),status);
        if (U_FAILURE(status)) {
            dataerrln("Error creating DecimalFormat - %s", u_errorName(status));
        } else {
            int64_t long_number = U_INT64_MAX; // -9223372036854775808L;
            // uint8_t bits[8];
            // memcpy(bits,&long_number,8);
            // for(int i=0;i<8;i++) {
            //   logln("bits: %02X", (unsigned int)bits[i]);
            // }
            UnicodeString expect = "9223372036854775807";
            UnicodeString result;
            FieldPosition pos;
            df.format(long_number, result, pos);
            if(U_FAILURE(status)||expect!=result) {
                dataerrln("%s:%d FAIL: expected '%s' got '%s' status %s on U_INT64_MAX",
                          __FILE__, __LINE__, CStr(expect)(), CStr(result)(), u_errorName(status));
            } else {
                logln("OK:  got expected '"+result+"' status "+UnicodeString(u_errorName(status),"")+" on U_INT64_MAX");
            }
        }
    }
    {
        UErrorCode status=U_ZERO_ERROR;
        DecimalFormat df(UnicodeString("0000000000000000000",""),status);
        if (U_FAILURE(status)) {
            dataerrln("Error creating DecimalFormat - %s", u_errorName(status));
        } else {
            int64_t long_number = 0;
            // uint8_t bits[8];
            // memcpy(bits,&long_number,8);
            // for(int i=0;i<8;i++) {
            //   logln("bits: %02X", (unsigned int)bits[i]);
            // }
            UnicodeString expect = "0000000000000000000";
            UnicodeString result;
            FieldPosition pos;
            df.format(long_number, result, pos);
            if(U_FAILURE(status)||expect!=result) {
                dataerrln("%s:%d FAIL: expected '%s' got '%s' status %s on 0",
                          __FILE__, __LINE__, CStr(expect)(), CStr(result)(), u_errorName(status));
            } else {
                logln("OK:  got expected '"+result+"' status "+UnicodeString(u_errorName(status),"")+" on 0");
            }
        }
    }
    {
        UErrorCode status=U_ZERO_ERROR;
        DecimalFormat df(UnicodeString("0000000000000000000",""),status);
        if (U_FAILURE(status)) {
            dataerrln("Error creating DecimalFormat - %s", u_errorName(status));
        } else {
            int64_t long_number = U_INT64_MIN + 1;
            UnicodeString expect = "-9223372036854775807";
            UnicodeString result;
            FieldPosition pos;
            df.format(long_number, result, pos);
            if(U_FAILURE(status)||expect!=result) {
                dataerrln("%s:%d FAIL: expected '%s' got '%s' status %s on -9223372036854775807",
                          __FILE__, __LINE__, CStr(expect)(), CStr(result)(), u_errorName(status));
            } else {
                logln("OK:  got expected '"+result+"' status "+UnicodeString(u_errorName(status),"")+" on -9223372036854775807");
            }
        }
    }
}