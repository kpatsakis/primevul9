MONGO_EXPORT void mongo_set_write_concern( mongo *conn,
        mongo_write_concern *write_concern ) {

    conn->write_concern = write_concern;
}