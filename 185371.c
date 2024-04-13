    TestOpaqueResourceDecoderImpl(absl::string_view name_field)
        : Config::OpaqueResourceDecoderImpl<Current>(ProtobufMessage::getStrictValidationVisitor(),
                                                     name_field) {}