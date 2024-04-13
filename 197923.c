        Status validateBSONIterative(Buffer* buffer) {
            std::deque<ValidationObjectFrame> frames;
            ValidationObjectFrame* curr = NULL;
            ValidationState::State state = ValidationState::BeginObj;

            while (state != ValidationState::Done) {
                switch (state) {
                case ValidationState::BeginObj:
                    frames.push_back(ValidationObjectFrame());
                    curr = &frames.back();
                    curr->setStartPosition(buffer->position());
                    curr->setIsCodeWithScope(false);
                    if (!buffer->readNumber<int>(&curr->expectedSize)) {
                        return Status(ErrorCodes::InvalidBSON,
                                      "bson size is larger than buffer size");
                    }
                    state = ValidationState::WithinObj;
                    // fall through
                case ValidationState::WithinObj: {
                    Status status = validateElementInfo(buffer, &state);
                    if (!status.isOK())
                        return status;
                    break;
                }
                case ValidationState::EndObj: {
                    int actualLength = buffer->position() - curr->startPosition();
                    if ( actualLength != curr->expectedSize ) {
                        return Status( ErrorCodes::InvalidBSON,
                                       "bson length doesn't match what we found" );
                    }
                    frames.pop_back();
                    if (frames.empty()) {
                        state = ValidationState::Done;
                    }
                    else {
                        curr = &frames.back();
                        if (curr->isCodeWithScope())
                            state = ValidationState::EndCodeWScope;
                        else
                            state = ValidationState::WithinObj;
                    }
                    break;
                }
                case ValidationState::BeginCodeWScope: {
                    frames.push_back(ValidationObjectFrame());
                    curr = &frames.back();
                    curr->setStartPosition(buffer->position());
                    curr->setIsCodeWithScope(true);
                    if ( !buffer->readNumber<int>( &curr->expectedSize ) )
                        return Status( ErrorCodes::InvalidBSON, "invalid bson CodeWScope size" );
                    Status status = buffer->readUTF8String( NULL );
                    if ( !status.isOK() )
                        return status;
                    state = ValidationState::BeginObj;
                    break;
                }
                case ValidationState::EndCodeWScope: {
                    int actualLength = buffer->position() - curr->startPosition();
                    if ( actualLength != curr->expectedSize ) {
                        return Status( ErrorCodes::InvalidBSON,
                                       "bson length for CodeWScope doesn't match what we found" );
                    }
                    frames.pop_back();
                    if (frames.empty())
                        return Status(ErrorCodes::InvalidBSON, "unnested CodeWScope");
                    curr = &frames.back();
                    state = ValidationState::WithinObj;
                    break;
                }
                case ValidationState::Done:
                    break;
                }
            }

            return Status::OK();
        }