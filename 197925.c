        Status validateElementInfo(Buffer* buffer, ValidationState::State* nextState) {
            Status status = Status::OK();

            char type;
            if ( !buffer->readNumber<char>(&type) )
                return Status( ErrorCodes::InvalidBSON, "invalid bson" );

            if ( type == EOO ) {
                *nextState = ValidationState::EndObj;
                return Status::OK();
            }

            StringData name;
            status = buffer->readCString( &name );
            if ( !status.isOK() )
                return status;

            switch ( type ) {
            case MinKey:
            case MaxKey:
            case jstNULL:
            case Undefined:
                return Status::OK();

            case jstOID:
                if ( !buffer->skip( sizeof(OID) ) )
                    return Status( ErrorCodes::InvalidBSON, "invalid bson" );
                return Status::OK();

            case NumberInt:
                if ( !buffer->skip( sizeof(int32_t) ) )
                    return Status( ErrorCodes::InvalidBSON, "invalid bson" );
                return Status::OK();

            case Bool:
                if ( !buffer->skip( sizeof(int8_t) ) )
                    return Status( ErrorCodes::InvalidBSON, "invalid bson" );
                return Status::OK();


            case NumberDouble:
            case NumberLong:
            case Timestamp:
            case Date:
                if ( !buffer->skip( sizeof(int64_t) ) )
                    return Status( ErrorCodes::InvalidBSON, "invalid bson" );
                return Status::OK();

            case DBRef:
                status = buffer->readUTF8String( NULL );
                if ( !status.isOK() )
                    return status;
                buffer->skip( sizeof(OID) );
                return Status::OK();

            case RegEx:
                status = buffer->readCString( NULL );
                if ( !status.isOK() )
                    return status;
                status = buffer->readCString( NULL );
                if ( !status.isOK() )
                    return status;

                return Status::OK();

            case Code:
            case Symbol:
            case String:
                status = buffer->readUTF8String( NULL );
                if ( !status.isOK() )
                    return status;
                return Status::OK();

            case BinData: {
                int sz;
                if ( !buffer->readNumber<int>( &sz ) )
                    return Status( ErrorCodes::InvalidBSON, "invalid bson" );
                if ( !buffer->skip( 1 + sz ) )
                    return Status( ErrorCodes::InvalidBSON, "invalid bson" );
                return Status::OK();
            }
            case CodeWScope:
                *nextState = ValidationState::BeginCodeWScope;
                return Status::OK();
            case Object:
            case Array:
                *nextState = ValidationState::BeginObj;
                return Status::OK();

            default:
                return Status( ErrorCodes::InvalidBSON, "invalid bson type" );
            }
        }