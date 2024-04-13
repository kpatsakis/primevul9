void NumberFormatTest::TestCurrencyUnit(void){
    UErrorCode ec = U_ZERO_ERROR;
    static const UChar USD[]  = u"USD";
    static const char USD8[]  =  "USD";
    static const UChar BAD[]  = u"???";
    static const UChar BAD2[] = u"??A";
    static const UChar XXX[]  = u"XXX";
    static const char XXX8[]  =  "XXX";
    CurrencyUnit cu(USD, ec);
    assertSuccess("CurrencyUnit", ec);

    assertEquals("getISOCurrency()", USD, cu.getISOCurrency());
    assertEquals("getSubtype()", USD8, cu.getSubtype());

    CurrencyUnit cu2(cu);
    if (!(cu2 == cu)){
        errln("CurrencyUnit copy constructed object should be same");
    }

    CurrencyUnit * cu3 = (CurrencyUnit *)cu.clone();
    if (!(*cu3 == cu)){
        errln("CurrencyUnit cloned object should be same");
    }
    CurrencyUnit bad(BAD, ec);
    assertSuccess("CurrencyUnit", ec);
    if (cu.getIndex() == bad.getIndex()) {
        errln("Indexes of different currencies should differ.");
    }
    CurrencyUnit bad2(BAD2, ec);
    assertSuccess("CurrencyUnit", ec);
    if (bad2.getIndex() != bad.getIndex()) {
        errln("Indexes of unrecognized currencies should be the same.");
    }
    if (bad == bad2) {
        errln("Different unrecognized currencies should not be equal.");
    }
    bad = bad2;
    if (bad != bad2) {
        errln("Currency unit assignment should be the same.");
    }
    delete cu3;

    // Test default constructor
    CurrencyUnit def;
    assertEquals("Default currency", XXX, def.getISOCurrency());
    assertEquals("Default currency as subtype", XXX8, def.getSubtype());

    // Test slicing
    MeasureUnit sliced1 = cu;
    MeasureUnit sliced2 = cu;
    assertEquals("Subtype after slicing 1", USD8, sliced1.getSubtype());
    assertEquals("Subtype after slicing 2", USD8, sliced2.getSubtype());
    CurrencyUnit restored1(sliced1, ec);
    CurrencyUnit restored2(sliced2, ec);
    assertSuccess("Restoring from MeasureUnit", ec);
    assertEquals("Subtype after restoring 1", USD8, restored1.getSubtype());
    assertEquals("Subtype after restoring 2", USD8, restored2.getSubtype());
    assertEquals("ISO Code after restoring 1", USD, restored1.getISOCurrency());
    assertEquals("ISO Code after restoring 2", USD, restored2.getISOCurrency());

    // Test copy constructor failure
    LocalPointer<MeasureUnit> meter(MeasureUnit::createMeter(ec));
    assertSuccess("Creating meter", ec);
    CurrencyUnit failure(*meter, ec);
    assertEquals("Copying from meter should fail", ec, U_ILLEGAL_ARGUMENT_ERROR);
    assertEquals("Copying should not give uninitialized ISO code", u"", failure.getISOCurrency());
}