_zip_dirent_finalize(struct zip_dirent *zde)
{
    if (zde->filename_len > 0) {
        free(zde->filename);
    }
    zde->filename = NULL;
    if (zde->extrafield_len > 0) {
        free(zde->extrafield);
    }
    zde->extrafield = NULL;
    if (zde->comment_len > 0) {
        free(zde->comment);
    }
    zde->comment = NULL;
}