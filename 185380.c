  CelValue Produce(ProtobufWkt::Arena* arena) override {
    return CelValue::CreateMessage(&metadata_, arena);
  }