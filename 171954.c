MONGO_EXPORT void gridfile_get_descriptor(gridfile* gf, bson* out) {
    *out = *gf->meta;
}