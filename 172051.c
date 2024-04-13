static mongo_message *mongo_message_create( size_t len , int id , int responseTo , int op ) {
    mongo_message *mm = ( mongo_message * )bson_malloc( len );

    if ( !id )
        id = rand();

    /* native endian (converted on send) */
    mm->head.len = ( int )len;
    mm->head.id = id;
    mm->head.responseTo = responseTo;
    mm->head.op = op;

    return mm;
}