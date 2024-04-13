zzip_disk_open(char* filename)
{
#  ifndef O_BINARY
#  define O_BINARY 0
#  endif
    struct stat st;
    if (stat (filename, &st) || !st.st_size) return 0;
    ___ int fd = open (filename, O_RDONLY|O_BINARY);
    if (fd <= 0) return 0;
    ___ ZZIP_DISK* disk = zzip_disk_mmap (fd);
    if (disk) return disk;
    ___ char* buffer = malloc (st.st_size);
    if (! buffer) return 0;
    if ((st.st_size == read (fd, buffer, st.st_size)) &&
	(disk = zzip_disk_new ())) 
    {
	disk->buffer = buffer;
	disk->endbuf = buffer+st.st_size;
	disk->mapped = -1;
    }else free (buffer);
    return disk; ____;____;____;
}