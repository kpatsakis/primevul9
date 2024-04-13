MONGO_EXPORT int mongo_validate_ns( mongo *conn, const char *ns ) {
    char *last = NULL;
    char *current = NULL;
    const char *db_name = ns;
    char *collection_name = NULL;
    char errmsg[64];
    int ns_len = 0;

    /* If the first character is a '.', fail. */
    if( *ns == '.' ) {
        __mongo_set_error( conn, MONGO_NS_INVALID, "ns cannot start with a '.'.", 0 );
        return MONGO_ERROR;
    }

    /* Find the division between database and collection names. */
    for( current = (char *)ns; *current != '\0'; current++ ) {
        if( *current == '.' ) {
            current++;
            break;
        }
    }

    /* Fail if the collection part starts with a dot. */
    if( *current == '.' ) {
        __mongo_set_error( conn, MONGO_NS_INVALID, "ns cannot start with a '.'.", 0 );
        return MONGO_ERROR;
    }

    /* Fail if collection length is 0.
     * or the ns doesn't contain a '.'. */
    if( *current == '\0' ) {
        __mongo_set_error( conn, MONGO_NS_INVALID, "Collection name missing.", 0 );
        return MONGO_ERROR;
    }


    /* Point to the beginning of the collection name. */
    collection_name = current;

    /* Ensure that the database name is greater than one char.*/
    if( collection_name - 1 == db_name ) {
        __mongo_set_error( conn, MONGO_NS_INVALID, "Database name missing.", 0 );
        return MONGO_ERROR;
    }

    /* Go back and validate the database name. */
    for( current = (char *)db_name; *current != '.'; current++ ) {
        switch( *current ) {
        case ' ':
        case '$':
        case '/':
        case '\\':
            __mongo_set_error( conn, MONGO_NS_INVALID,
                               "Database name may not contain ' ', '$', '/', or '\\'", 0 );
            return MONGO_ERROR;
        default:
            break;
        }

        ns_len++;
    }

    /* Add one to the length for the '.' character. */
    ns_len++;

    /* Now validate the collection name. */
    for( current = collection_name; *current != '\0'; current++ ) {

        /* Cannot have two consecutive dots. */
        if( last && *last == '.' && *current == '.' ) {
            __mongo_set_error( conn, MONGO_NS_INVALID,
                               "Collection may not contain two consecutive '.'", 0 );
            return MONGO_ERROR;
        }

        /* Cannot contain a '$' */
        if( *current == '$' ) {
            __mongo_set_error( conn, MONGO_NS_INVALID,
                               "Collection may not contain '$'", 0 );
            return MONGO_ERROR;
        }

        last = current;
        ns_len++;
    }

    if( ns_len > 128 ) {
        bson_sprintf( errmsg, "Namespace too long; has %d but must <= 128.",
                      ns_len );
        __mongo_set_error( conn, MONGO_NS_INVALID, errmsg, 0 );
        return MONGO_ERROR;
    }

    /* Cannot end with a '.' */
    if( *(current - 1) == '.' ) {
        __mongo_set_error( conn, MONGO_NS_INVALID,
                           "Collection may not end with '.'", 0 );
        return MONGO_ERROR;
    }

    return MONGO_OK;
}