MATCHER_P(JsonStringEq, expected, "") {
  const bool equal = TestUtility::jsonStringEqual(arg, expected);
  if (!equal) {
    *result_listener << "\n"
                     << TestUtility::addLeftAndRightPadding("Expected JSON string:") << "\n"
                     << expected
                     << TestUtility::addLeftAndRightPadding("is not equal to actual JSON string:")
                     << "\n"
                     << arg << TestUtility::addLeftAndRightPadding("") // line full of padding
                     << "\n";
  }
  return equal;
}