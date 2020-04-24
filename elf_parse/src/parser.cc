#include <parser.h>
const char *flat = "./flat.bin";
const char *kernel_file = "./deliceos.elf";


int main(){
    File file(flat);
    uint8_t *faddr = file.openfile(kernel_file);
    //Initialize ELF header
    Elf32_Ehdr *hdr = (Elf32_Ehdr*)faddr;
    if(hdr->e_ident[1] != 'E' ||
       hdr->e_ident[2] != 'L' ||
       hdr->e_ident[3] != 'F'){
        printf("Failed to parse ELF file (file signature has corrupted)\n");
        exit(1);
    }
    //Get section offset in the ELF header
    uint32_t sh_off = hdr->e_shoff;
    //Get the number of entries in section header table
    uint8_t entry_sh = hdr->e_shnum;

    //Initialize struct to storing offset and size of each sector
    std::vector<FileHeader> fheader;
    if(!(entry_sh > 0)){
        printf("There is no any entry in the section header table\n");
        exit(1);
    }
    for (int ii = 0; ii < entry_sh; ii++){
        Elf32_Shdr *shdr = 
            (Elf32_Shdr*)(faddr + sh_off + (sizeof(Elf32_Shdr) * ii));
            if(shdr->sh_offset > 0 && (shdr->sh_offset < sh_off)) {
                uint32_t offset = shdr->sh_offset;
                uint32_t size = shdr->sh_size;
                fheader.push_back(initheader(offset,size));
            }
    }
    
     assert (fheader.begin() != fheader.end());
     //Get offset of first section.
     std::vector<FileHeader>::iterator minoffset = 
         std::min_element(fheader.begin(), fheader.end(), comparator);
     //Get offset of last section.
     std::vector<FileHeader>::iterator maxoffset = 
         std::max_element(fheader.begin(), fheader.end(), comparator);
     uint32_t image_base = minoffset->offset;
     uint32_t image_end = maxoffset->offset + maxoffset->size;


     //Write bytes from first until last section
     for(uint32_t ii = image_base; ii < image_end; ii++){
        uint8_t *file_block = (faddr + ii);
        file.writefile(file_block);
     }

}
