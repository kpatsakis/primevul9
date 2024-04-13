const char* attrString(int32_t attrId) {
  switch (attrId) {
    case UNUM_INTEGER_FIELD: return "integer";
    case UNUM_FRACTION_FIELD: return "fraction";
    case UNUM_DECIMAL_SEPARATOR_FIELD: return "decimal separator";
    case UNUM_EXPONENT_SYMBOL_FIELD: return "exponent symbol";
    case UNUM_EXPONENT_SIGN_FIELD: return "exponent sign";
    case UNUM_EXPONENT_FIELD: return "exponent";
    case UNUM_GROUPING_SEPARATOR_FIELD: return "grouping separator";
    case UNUM_CURRENCY_FIELD: return "currency";
    case UNUM_PERCENT_FIELD: return "percent";
    case UNUM_PERMILL_FIELD: return "permille";
    case UNUM_SIGN_FIELD: return "sign";
    default: return "";
  }
}