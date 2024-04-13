void NumberFormatTest::Test10727_RoundingZero() {
    IcuTestErrorCode status(*this, "Test10727_RoundingZero");
    DecimalQuantity dq;
    dq.setToDouble(-0.0);
    assertTrue("", dq.isNegative());
    dq.roundToMagnitude(0, UNUM_ROUND_HALFEVEN, status);
    assertTrue("", dq.isNegative());
}