key_get_es_version(uint8_t version[2])
{
    struct es_version {
        uint8_t es_version[2];
        const char *name;
    };

    const int num_versions = 2;
    struct es_version es_versions[] = {
        {{0x00, 0x01}, "X25519-XSalsa20Poly1305"},
        {{0x00, 0x02}, "X25519-XChacha20Poly1305"},
    };
    int i;
    for(i=0; i < num_versions; i++){
        if(es_versions[i].es_version[0] == version[0] &&
           es_versions[i].es_version[1] == version[1]){
            return es_versions[i].name;
        }
    }
    return NULL;
}