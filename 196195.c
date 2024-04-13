void NumberFormatTest::Test13056_GroupingSize() {
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormat df(u"#,##0", status);
    if (!assertSuccess("", status)) return;
    assertEquals("Primary grouping should return 3", 3, df.getGroupingSize());
    assertEquals("Secondary grouping should return 0", 0, df.getSecondaryGroupingSize());
    df.setSecondaryGroupingSize(3);
    assertEquals("Primary grouping should still return 3", 3, df.getGroupingSize());
    assertEquals("Secondary grouping should round-trip", 3, df.getSecondaryGroupingSize());
    df.setGroupingSize(4);
    assertEquals("Primary grouping should return 4", 4, df.getGroupingSize());
    assertEquals("Secondary should remember explicit setting and return 3", 3, df.getSecondaryGroupingSize());
}