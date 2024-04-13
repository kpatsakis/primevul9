MONGO_EXPORT void gridfs_dispose(gridfs* gfs) {
    free(gfs);
}