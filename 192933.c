static inline unsigned short snd_seq_file_flags(struct file *file)
{
        switch (file->f_mode & (FMODE_READ | FMODE_WRITE)) {
        case FMODE_WRITE:
                return SNDRV_SEQ_LFLG_OUTPUT;
        case FMODE_READ:
                return SNDRV_SEQ_LFLG_INPUT;
        default:
                return SNDRV_SEQ_LFLG_OPEN;
        }
}