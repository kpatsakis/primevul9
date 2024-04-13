  void UrlParser::finish()
  {
    switch(_state)
    {
      case state_0:
        break;

      case state_key:
        if (!_key.empty())
        {
          _q.add(_key);
          _key.clear();
        }
        break;

      case state_value:
        _q.add(_key, _value);
        _key.clear();
        _value.clear();
        break;

      case state_keyesc:
      case state_valueesc:
        if (_cnt == 0)
        {
          if (_state == state_keyesc)
          {
            _key += '%';
            _q.add(_key);
          }
          else
          {
            _value += '%';
            _q.add(_key, _value);
          }
        }
        else
        {
          if (_state == state_keyesc)
          {
            _key += static_cast<char>(_v);
            _q.add(_key);
          }
          else
          {
            _value += static_cast<char>(_v);
            _q.add(_key, _value);
          }
        }

        _value.clear();
        _key.clear();
        _cnt = 0;
        _v = 0;
        break;
    }

  }