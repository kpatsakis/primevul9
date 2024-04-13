    Value::AutoPtr Iptcdatum::getValue() const
    {
        return value_.get() == 0 ? Value::AutoPtr(0) : value_->clone();
    }