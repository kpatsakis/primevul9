void NumberFormatTest::TestUFormattable(void) {
  {
    // test that a default formattable is equal to Formattable()
    UErrorCode status = U_ZERO_ERROR;
    LocalUFormattablePointer defaultUFormattable(ufmt_open(&status));
    assertSuccess("calling umt_open", status);
    Formattable defaultFormattable;
    assertTrue((UnicodeString)"comparing ufmt_open() with Formattable()",
               (defaultFormattable
                == *(Formattable::fromUFormattable(defaultUFormattable.getAlias()))));
    assertTrue((UnicodeString)"comparing ufmt_open() with Formattable()",
               (defaultFormattable
                == *(Formattable::fromUFormattable(defaultUFormattable.getAlias()))));
    assertTrue((UnicodeString)"comparing Formattable() round tripped through UFormattable",
               (defaultFormattable
                == *(Formattable::fromUFormattable(defaultFormattable.toUFormattable()))));
    assertTrue((UnicodeString)"comparing &Formattable() round tripped through UFormattable",
               ((&defaultFormattable)
                == Formattable::fromUFormattable(defaultFormattable.toUFormattable())));
    assertFalse((UnicodeString)"comparing &Formattable() with ufmt_open()",
               ((&defaultFormattable)
                == Formattable::fromUFormattable(defaultUFormattable.getAlias())));
    testFormattableAsUFormattable(__FILE__, __LINE__, defaultFormattable);
  }
  // test some random Formattables
  {
    Formattable f(ucal_getNow(), Formattable::kIsDate);
    testFormattableAsUFormattable(__FILE__, __LINE__,  f);
  }
  {
    Formattable f((double)1.61803398874989484820); // golden ratio
    testFormattableAsUFormattable(__FILE__, __LINE__,  f);
  }
  {
    Formattable f((int64_t)80994231587905127LL); // weight of the moon, in kilotons http://solarsystem.nasa.gov/planets/profile.cfm?Display=Facts&Object=Moon
    testFormattableAsUFormattable(__FILE__, __LINE__,  f);
  }
  {
    Formattable f((int32_t)4); // random number, source: http://www.xkcd.com/221/
    testFormattableAsUFormattable(__FILE__, __LINE__,  f);
  }
  {
    Formattable f("Hello world."); // should be invariant?
    testFormattableAsUFormattable(__FILE__, __LINE__,  f);
  }
  {
    UErrorCode status2 = U_ZERO_ERROR;
    Formattable f(StringPiece("73476730924573500000000.0"), status2); // weight of the moon, kg
    assertSuccess("Constructing a StringPiece", status2);
    testFormattableAsUFormattable(__FILE__, __LINE__,  f);
  }
  {
    UErrorCode status2 = U_ZERO_ERROR;
    UObject *obj = new Locale();
    Formattable f(obj);
    assertSuccess("Constructing a Formattable from a default constructed Locale()", status2);
    testFormattableAsUFormattable(__FILE__, __LINE__,  f);
  }
  {
    const Formattable array[] = {
      Formattable(ucal_getNow(), Formattable::kIsDate),
      Formattable((int32_t)4),
      Formattable((double)1.234),
    };

    Formattable fa(array, 3);
    testFormattableAsUFormattable(__FILE__, __LINE__, fa);
  }
}