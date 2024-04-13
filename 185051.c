absl::optional<CelValue> ResponseWrapper::operator[](CelValue key) const {
  if (!key.IsString()) {
    return {};
  }
  auto value = key.StringOrDie().value();
  if (value == Code) {
    auto code = info_.responseCode();
    if (code.has_value()) {
      return CelValue::CreateInt64(code.value());
    }
    return {};
  } else if (value == Size) {
    return CelValue::CreateInt64(info_.bytesSent());
  } else if (value == Headers) {
    return CelValue::CreateMap(&headers_);
  } else if (value == Trailers) {
    return CelValue::CreateMap(&trailers_);
  } else if (value == Flags) {
    return CelValue::CreateInt64(info_.responseFlags());
  } else if (value == GrpcStatus) {
    auto const& optional_status = Grpc::Common::getGrpcStatus(
        trailers_.value_ ? *trailers_.value_ : *Http::StaticEmptyHeaders::get().response_trailers,
        headers_.value_ ? *headers_.value_ : *Http::StaticEmptyHeaders::get().response_headers,
        info_);
    if (optional_status.has_value()) {
      return CelValue::CreateInt64(optional_status.value());
    }
    return {};
  } else if (value == TotalSize) {
    return CelValue::CreateInt64(info_.bytesSent() +
                                 (headers_.value_ ? headers_.value_->byteSize() : 0) +
                                 (trailers_.value_ ? trailers_.value_->byteSize() : 0));
  } else if (value == CodeDetails) {
    const absl::optional<std::string>& details = info_.responseCodeDetails();
    if (details.has_value()) {
      return CelValue::CreateString(&details.value());
    }
    return {};
  }
  return {};
}