  CelValue Produce(ProtobufWkt::Arena* arena) override {
    // Producer is unique per evaluation arena since activation is re-created.
    arena_ = arena;
    return CelValue::CreateMap(this);
  }