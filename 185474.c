MATCHER_P(ProtoEq, expected, "") {
  const bool equal =
      TestUtility::protoEqual(arg, expected, /*ignore_repeated_field_ordering=*/false);
  if (!equal) {
    *result_listener << "\n"
                     << "==========================Expected proto:===========================\n"
                     << expected.DebugString()
                     << "------------------is not equal to actual proto:---------------------\n"
                     << arg.DebugString()
                     << "====================================================================\n";
  }
  return equal;
}