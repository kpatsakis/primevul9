void NumberFormatTest::TestFormattableSize(void) {
  if(sizeof(Formattable) > 112) {
    errln("Error: sizeof(Formattable)=%d, 112=%d\n",
          sizeof(Formattable), 112);
  } else if(sizeof(Formattable) < 112) {
    logln("Warning: sizeof(Formattable)=%d, 112=%d\n",
        sizeof(Formattable), 112);
  } else {
    logln("sizeof(Formattable)=%d, 112=%d\n",
        sizeof(Formattable), 112);
  }
}