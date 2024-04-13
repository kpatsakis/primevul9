      explicit UrlParser(QueryParams& q)
        : _q(q),
          _state(state_0),
          _cnt(0),
          _v(0)
      { }