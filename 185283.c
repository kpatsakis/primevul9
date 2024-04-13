bool HeaderUtility::isConnectResponse(const RequestHeaderMap* request_headers,
                                      const ResponseHeaderMap& response_headers) {
  return request_headers && isConnect(*request_headers) &&
         static_cast<Http::Code>(Http::Utility::getResponseStatus(response_headers)) ==
             Http::Code::OK;
}