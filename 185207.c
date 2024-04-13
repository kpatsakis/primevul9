MATCHER_P2(ProtoEqIgnoringField, expected, ignored_field, "") {
  const bool equal = TestUtility::protoEqualIgnoringField(arg, expected, ignored_field);
  if (!equal) {
    std::string but_ignoring = absl::StrCat("(but ignoring ", ignored_field, ")");
    *result_listener << "\n"
                     << TestUtility::addLeftAndRightPadding("Expected proto:") << "\n"
                     << TestUtility::addLeftAndRightPadding(but_ignoring) << "\n"
                     << expected.DebugString()
                     << TestUtility::addLeftAndRightPadding("is not equal to actual proto:") << "\n"
                     << arg.DebugString()
                     << TestUtility::addLeftAndRightPadding("") // line full of padding
                     << "\n";
  }
  return equal;
}