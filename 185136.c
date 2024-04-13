HeaderMapImplUtility::getAllHeaderMapImplInfo() {
  std::vector<HeaderMapImplUtility::HeaderMapImplInfo> ret;
  ret.push_back(makeHeaderMapImplInfo<RequestHeaderMapImpl>("request header map"));
  ret.push_back(makeHeaderMapImplInfo<RequestTrailerMapImpl>("request trailer map"));
  ret.push_back(makeHeaderMapImplInfo<ResponseHeaderMapImpl>("response header map"));
  ret.push_back(makeHeaderMapImplInfo<ResponseTrailerMapImpl>("response trailer map"));
  return ret;
}