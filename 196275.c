void NumberFormatTest::verifyFieldPositionIterator(
        NumberFormatTest_Attributes *expected, FieldPositionIterator &iter) {
    int32_t idx = 0;
    FieldPosition fp;
    while (iter.next(fp)) {
        if (expected[idx].spos == -1) {
            errln("Iterator should have ended. got %d", fp.getField());
            return;
        }
        assertEquals("id", expected[idx].id, fp.getField());
        assertEquals("start", expected[idx].spos, fp.getBeginIndex());
        assertEquals("end", expected[idx].epos, fp.getEndIndex());
        ++idx;
    }
    if (expected[idx].spos != -1) {
        errln("Premature end of iterator. expected %d", expected[idx].id);
    }
}