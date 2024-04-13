  ResponseWrapper(Protobuf::Arena& arena, const Http::ResponseHeaderMap* headers,
                  const Http::ResponseTrailerMap* trailers, const StreamInfo::StreamInfo& info)
      : headers_(arena, headers), trailers_(arena, trailers), info_(info) {}