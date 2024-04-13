void NumberFormatTest::TestMultiplierWithScale() {
    IcuTestErrorCode status(*this, "TestMultiplierWithScale");

    // Test magnitude combined with multiplier, as shown in API docs
    DecimalFormat df("0", {"en", status}, status);
    if (status.isSuccess()) {
        df.setMultiplier(5);
        df.setMultiplierScale(-1);
        expect2(df, 100, u"50"); // round-trip test
    }
}