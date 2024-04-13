static bcon_error_t bson_bcon_key_value(bson *b, const char *key, const char *typespec, const bcon bci) {
    bcon_error_t ret = BCON_OK;
    bson_oid_t oid;
    char ptype = typespec ? typespec[1] : '_';
    char utype = typespec ? typespec[2] : '_';
    switch (ptype) {
    case '_': /* kv(b, key, utype, bci) */
        switch (utype) {
        case '_': /* fall through */
        case 's': bson_append_string( b, key, bci.s ); break; /* common case */
        case 'f': bson_append_double( b, key, bci.f ); break;
        case 'D':
            bson_append_start_object( b, key );
            ret = bson_append_bcon( b, bci.D );
            bson_append_finish_object( b );
            break;
        case 'A':
            bson_append_start_array( b, key );
            ret = bson_append_bcon_array( b, bci.A );
            bson_append_finish_array( b );
            break;
        case 'o': if (*bci.o == '\0') bson_oid_gen( &oid ); else bson_oid_from_string( &oid, bci.o ); bson_append_oid( b, key, &oid ); break;
        case 'b': bson_append_bool( b, key, bci.b ); break;
        case 't': bson_append_time_t( b, key, bci.t ); break;
        case 'v': bson_append_null( b, key ); break; /* void */
        case 'x': bson_append_symbol( b, key, bci.x ); break;
        case 'i': bson_append_int( b, key, bci.i ); break;
        case 'l': bson_append_long( b, key, bci.l ); break;
        default: printf("\nptype:'%c' utype:'%c'\n", ptype, utype); assert(NOT_REACHED); break;
        }
        break;
    case 'R': /* krv(b, key, utype, bci) */
        switch (utype) {
        case 'f': bson_append_double( b, key, *bci.Rf ); break;
        case 's': bson_append_string( b, key, bci.Rs ); break;
        case 'D':
            bson_append_start_object( b, key );
            ret = bson_append_bcon( b, bci.RD );
            bson_append_finish_object( b );
            break;
        case 'A':
            bson_append_start_array( b, key );
            ret = bson_append_bcon_array( b, bci.RA );
            bson_append_finish_array( b );
            break;
        case 'o': if (*bci.o == '\0') bson_oid_gen( &oid ); else bson_oid_from_string( &oid, bci.o ); bson_append_oid( b, key, &oid ); break;
        case 'b': bson_append_bool( b, key, *bci.Rb ); break;
        case 't': bson_append_time_t( b, key, *bci.Rt ); break;
        case 'x': bson_append_symbol( b, key, bci.Rx ); break;
        case 'i': bson_append_int( b, key, *bci.Ri ); break;
        case 'l': bson_append_long( b, key, *bci.Rl ); break;
        default: printf("\nptype:'%c' utype:'%c'\n", ptype, utype); assert(NOT_REACHED); break;
        }
        break;
    case 'P': /* kpv(b, key, utype, bci) */
        if (*bci.Pv != 0) {
            switch (utype) {
            case 'f': bson_append_double( b, key, **bci.Pf ); break;
            case 's': bson_append_string( b, key, *bci.Ps ); break;
            case 'D':
                bson_append_start_object( b, key );
                ret = bson_append_bcon( b, *bci.PD );
                bson_append_finish_object( b );
                break;
            case 'A':
                bson_append_start_array( b, key );
                ret = bson_append_bcon_array( b, *bci.PA );
                bson_append_finish_array( b );
                break;
            case 'o': if (**bci.Po == '\0') bson_oid_gen( &oid );
                else bson_oid_from_string( &oid, *bci.Po );
                bson_append_oid( b, key, &oid );
                break;
            case 'b': bson_append_bool( b, key, **bci.Pb ); break;
            case 't': bson_append_time_t( b, key, **bci.Pt ); break;
            case 'x': if (*bci.Px != 0) bson_append_symbol( b, key, *bci.Px ); break;
            case 'i': bson_append_int( b, key, **bci.Pi ); break;
            case 'l': bson_append_long( b, key, **bci.Pl ); break;
            default: printf("\nptype:'%c' utype:'%c'\n", ptype, utype); assert(NOT_REACHED); break;
            }
        }
        break;
    default:
        printf("\nptype:'%c' utype:'%c'\n", ptype, utype); assert(NOT_REACHED);
        break;
    }
    return ret;
}