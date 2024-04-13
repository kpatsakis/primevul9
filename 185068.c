  RequestWrapper(Protobuf::Arena& arena, const Http::RequestHeaderMap* headers,
                 const StreamInfo::StreamInfo& info)
      : headers_(arena, headers), info_(info) {}