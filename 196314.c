NumberFormatTest::TestCoverage(void){
    StubNumberFormat stub;
    UnicodeString agent("agent");
    FieldPosition pos;
    int64_t num = 4;
    if (stub.format(num, agent, pos) != UnicodeString("agent3")){
        errln("NumberFormat::format(int64, UnicodString&, FieldPosition&) should delegate to (int32, ,)");
    };
}