    const Value& Iptcdatum::value() const
    {
        if (value_.get() == 0) throw Error(8);
        return *value_;
    }