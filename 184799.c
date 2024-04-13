_idn2_bidi (const uint32_t * label, size_t llen)
{
  int bc;
  int endok = 1;

  if (!_isBidi (label, llen))
    return IDN2_OK;

  // 2.1
  switch ((bc = uc_bidi_category (*label))) {
    case UC_BIDI_L:
      // check 2.5 & 2.6
      for (size_t it = 1; it < llen; it++) {
	bc = uc_bidi_category(label[it]);

	if (bc == UC_BIDI_L || bc == UC_BIDI_EN || bc == UC_BIDI_NSM) {
	  endok = 1;
	} else {
	  if (bc != UC_BIDI_ES && bc != UC_BIDI_CS && bc != UC_BIDI_ET && bc != UC_BIDI_ON && bc != UC_BIDI_BN) {
	    /* printf("LTR label contains invalid code point\n"); */
	    return IDN2_BIDI;
	  }
	  endok = 0;
	}
      }
      /* printf("LTR label ends with invalid code point\n"); */
      return endok ? IDN2_OK : IDN2_BIDI;

    case UC_BIDI_R:
    case UC_BIDI_AL:
      // check 2.2, 2.3, 2.4
      /* printf("Label[0]=%04X: %s\n", label[0], uc_bidi_category_name(bc)); */
      for (size_t it = 1; it < llen; it++) {
	bc = uc_bidi_category(label[it]);

	/* printf("Label[%zu]=%04X: %s\n", it, label[it], uc_bidi_category_name(bc)); */
	if (bc == UC_BIDI_R || bc == UC_BIDI_AL || bc == UC_BIDI_EN || bc == UC_BIDI_AN || bc == UC_BIDI_NSM) {
	  endok = 1;
	} else {
	  if (bc != UC_BIDI_ES && bc != UC_BIDI_CS && bc != UC_BIDI_ET && bc != UC_BIDI_ON && bc != UC_BIDI_BN) {
	    /* printf("RTL label contains invalid code point\n"); */
	    return IDN2_BIDI;
	  }
	  endok = 0;
	}
      }
      /* printf("RTL label ends with invalid code point\n"); */
      return endok ? IDN2_OK : IDN2_BIDI;

    default:
      /* printf("Label begins with invalid BIDI class %s\n", uc_bidi_category_name(bc)); */
      return IDN2_BIDI;
  }
}