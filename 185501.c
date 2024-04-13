bool HeaderUtility::isConnect(const RequestHeaderMap& headers) {
  return headers.Method() && headers.Method()->value() == Http::Headers::get().MethodValues.Connect;
}