PLIST_API void plist_to_bin(plist_t plist, char **plist_bin, uint32_t * length)
{
    ptrarray_t* objects = NULL;
    hashtable_t* ref_table = NULL;
    struct serialize_s ser_s;
    uint8_t offset_size = 0;
    uint8_t ref_size = 0;
    uint64_t num_objects = 0;
    uint64_t root_object = 0;
    uint64_t offset_table_index = 0;
    bytearray_t *bplist_buff = NULL;
    uint64_t i = 0;
    uint8_t *buff = NULL;
    uint64_t *offsets = NULL;
    bplist_trailer_t trailer;
    //for string
    long len = 0;
    long items_read = 0;
    long items_written = 0;
    uint16_t *unicodestr = NULL;
    uint64_t objects_len = 0;
    uint64_t buff_len = 0;

    //check for valid input
    if (!plist || !plist_bin || *plist_bin || !length)
        return;

    //list of objects
    objects = ptr_array_new(256);
    //hashtable to write only once same nodes
    ref_table = hash_table_new(plist_data_hash, plist_data_compare, free);

    //serialize plist
    ser_s.objects = objects;
    ser_s.ref_table = ref_table;
    serialize_plist(plist, &ser_s);

    //now stream to output buffer
    offset_size = 0;			//unknown yet
    objects_len = objects->len;
    ref_size = get_needed_bytes(objects_len);
    num_objects = objects->len;
    root_object = 0;			//root is first in list
    offset_table_index = 0;		//unknown yet

    //setup a dynamic bytes array to store bplist in
    bplist_buff = byte_array_new();

    //set magic number and version
    byte_array_append(bplist_buff, BPLIST_MAGIC, BPLIST_MAGIC_SIZE);
    byte_array_append(bplist_buff, BPLIST_VERSION, BPLIST_VERSION_SIZE);

    //write objects and table
    offsets = (uint64_t *) malloc(num_objects * sizeof(uint64_t));
    assert(offsets != NULL);
    for (i = 0; i < num_objects; i++)
    {

        plist_data_t data = plist_get_data(ptr_array_index(objects, i));
        offsets[i] = bplist_buff->len;

        switch (data->type)
        {
        case PLIST_BOOLEAN:
            buff = (uint8_t *) malloc(sizeof(uint8_t));
            buff[0] = data->boolval ? BPLIST_TRUE : BPLIST_FALSE;
            byte_array_append(bplist_buff, buff, sizeof(uint8_t));
            free(buff);
            break;

        case PLIST_UINT:
            if (data->length == 16) {
                write_uint(bplist_buff, data->intval);
            } else {
                write_int(bplist_buff, data->intval);
            }
            break;

        case PLIST_REAL:
            write_real(bplist_buff, data->realval);
            break;

        case PLIST_KEY:
        case PLIST_STRING:
            len = strlen(data->strval);
            if ( is_ascii_string(data->strval, len) )
            {
                write_string(bplist_buff, data->strval);
            }
            else
            {
                unicodestr = plist_utf8_to_utf16(data->strval, len, &items_read, &items_written);
                write_unicode(bplist_buff, unicodestr, items_written);
                free(unicodestr);
            }
            break;
        case PLIST_DATA:
            write_data(bplist_buff, data->buff, data->length);
        case PLIST_ARRAY:
            write_array(bplist_buff, ptr_array_index(objects, i), ref_table, ref_size);
            break;
        case PLIST_DICT:
            write_dict(bplist_buff, ptr_array_index(objects, i), ref_table, ref_size);
            break;
        case PLIST_DATE:
            write_date(bplist_buff, data->realval);
            break;
        case PLIST_UID:
            write_uid(bplist_buff, data->intval);
            break;
        default:
            break;
        }
    }

    //free intermediate objects
    ptr_array_free(objects);
    hash_table_destroy(ref_table);

    //write offsets
    buff_len = bplist_buff->len;
    offset_size = get_needed_bytes(buff_len);
    offset_table_index = bplist_buff->len;
    for (i = 0; i < num_objects; i++) {
        uint64_t offset = be64toh(offsets[i]);
        byte_array_append(bplist_buff, (uint8_t*)&offset + (sizeof(uint64_t) - offset_size), offset_size);
    }
    free(offsets);

    //setup trailer
    memset(trailer.unused, '\0', sizeof(trailer.unused));
    trailer.offset_size = offset_size;
    trailer.ref_size = ref_size;
    trailer.num_objects = be64toh(num_objects);
    trailer.root_object_index = be64toh(root_object);
    trailer.offset_table_offset = be64toh(offset_table_index);

    byte_array_append(bplist_buff, &trailer, sizeof(bplist_trailer_t));

    //set output buffer and size
    *plist_bin = bplist_buff->data;
    *length = bplist_buff->len;

    bplist_buff->data = NULL; // make sure we don't free the output buffer
    byte_array_free(bplist_buff);
}