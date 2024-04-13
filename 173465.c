void PackLinuxElf64::un_DT_INIT(
    unsigned old_dtinit,
    Elf64_Phdr const *const phdro,
    Elf64_Phdr const *const dynhdr,  // in phdri
    OutputFile *fo,
    unsigned is_asl
)
{
    // DT_INIT must be restored.
    // If android_shlib, then the asl_delta relocations must be un-done.
     upx_uint64_t dt_pltrelsz(0), dt_jmprel(0);
     upx_uint64_t dt_relasz(0), dt_rela(0);
     upx_uint64_t const dyn_len = get_te64(&dynhdr->p_filesz);
     upx_uint64_t const dyn_off = get_te64(&dynhdr->p_offset);
     if ((unsigned long)file_size < (dyn_len + dyn_off)) {
         char msg[50]; snprintf(msg, sizeof(msg),
                 "bad PT_DYNAMIC .p_filesz %#lx", (long unsigned)dyn_len);
         throwCantUnpack(msg);
     }
     fi->seek(dyn_off, SEEK_SET);
     fi->readx(ibuf, dyn_len);
     Elf64_Dyn *dyn = (Elf64_Dyn *)(void *)ibuf;
     dynseg = dyn; invert_pt_dynamic(dynseg,
         umin(dyn_len, file_size - dyn_off));
     for (unsigned j2= 0; j2 < dyn_len; ++dyn, j2 += sizeof(*dyn)) {
         upx_uint64_t const tag = get_te64(&dyn->d_tag);
         upx_uint64_t       val = get_te64(&dyn->d_val);
         if (is_asl) switch (tag) {
         case Elf64_Dyn::DT_RELASZ:   { dt_relasz   = val; } break;
         case Elf64_Dyn::DT_RELA:     { dt_rela     = val; } break;
         case Elf64_Dyn::DT_PLTRELSZ: { dt_pltrelsz = val; } break;
         case Elf64_Dyn::DT_JMPREL:   { dt_jmprel   = val; } break;

         case Elf64_Dyn::DT_PLTGOT:
         case Elf64_Dyn::DT_PREINIT_ARRAY:
         case Elf64_Dyn::DT_INIT_ARRAY:
         case Elf64_Dyn::DT_FINI_ARRAY:
         case Elf64_Dyn::DT_FINI: {
             set_te64(&dyn->d_val, val - asl_delta);
         }; break;
         } // end switch() on tag when is_asl
         if (upx_dt_init == tag) {
             if (Elf64_Dyn::DT_INIT == tag) {
                 set_te64(&dyn->d_val, old_dtinit);
                 if (!old_dtinit) { // compressor took the slot
                     dyn->d_tag = Elf64_Dyn::DT_NULL;
                     dyn->d_val = 0;
                 }
             }
             else if (Elf64_Dyn::DT_INIT_ARRAY    == tag
             ||       Elf64_Dyn::DT_PREINIT_ARRAY == tag) {
                 if (val < load_va || (long unsigned)file_size < (long unsigned)val) {
                     char msg[50]; snprintf(msg, sizeof(msg),
                             "Bad Dynamic tag %#lx %#lx",
                             (long unsigned)tag, (long unsigned)val);
                     throwCantUnpack(msg);
                 }
                 set_te64(&ibuf[val - load_va], old_dtinit
                     + (is_asl ? asl_delta : 0));  // counter-act unRel64
             }
         }
     }
     if (fo) { // Write updated dt_*.val
         upx_uint64_t dyn_offo = get_te64(&phdro[dynhdr - phdri].p_offset);
         fo->seek(dyn_offo, SEEK_SET);
         fo->rewrite(ibuf, dyn_len);
     }
     if (is_asl) {
         lowmem.alloc(xct_off);
         fi->seek(0, SEEK_SET);
         fi->read(lowmem, xct_off);  // contains relocation tables
         if (dt_relasz && dt_rela) {
             Elf64_Rela *const rela0 = (Elf64_Rela *)lowmem.subref(
                 "bad Rela offset", dt_rela, dt_relasz);
             unRela64(dt_rela, rela0, dt_relasz, ibuf, load_va, old_dtinit, fo);
         }
         if (dt_pltrelsz && dt_jmprel) { // FIXME:  overlap w/ DT_REL ?
             Elf64_Rela *const jmp0 = (Elf64_Rela *)lowmem.subref(
                 "bad Jmprel offset", dt_jmprel, dt_pltrelsz);
             unRela64(dt_jmprel, jmp0, dt_pltrelsz, ibuf, load_va, old_dtinit, fo);
         }
         // Modified relocation tables are re-written by unRela64
     }
}