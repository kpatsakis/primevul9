    Iptcdatum& Iptcdatum::operator=(const uint16_t& value)
    {
        UShortValue::AutoPtr v(new UShortValue);
        v->value_.push_back(value);
        value_ = v;
        return *this;
    }