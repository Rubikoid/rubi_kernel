#include <kernel/vfs/fs/initrd.h>
#include <kernel/memory/heap.h>
#include <lib/stdio.h>
#include <lib/string.h>

struct initrd_status_t ird_stat = {0};

void initrd_init(void *ptr) {
    ird_stat.head = ptr;
    if(ird_stat.head->magic != HEADMAGIC) {
        printf("Bad head magic\n");
        return;
    }
    printf("offset: %x\n", ird_stat.head->offset);
    ird_stat.root = ptr + ird_stat.head->offset;
    if(ird_stat.root->magic != FILEMAGIC) {
        printf("Bad root magic\n");
        return;
    }
    {
        char *eone_name = kmalloc(ird_stat.root->name_size + 1);
        memcpy(eone_name, (void *)ird_stat.root + sizeof(struct initrd_file_head_t), ird_stat.root->name_size);
        eone_name[ird_stat.root->name_size] = '\0';
        printf("RootNameCheck: %s\n", eone_name);
        kfree(eone_name);
    }
}

void resolve_path(char *path) {
    char *curr_name = 0;
    int i=0;
    while(path[i] != '\0') {
        
    }
}


void initrd_test(void *ptr) {
    printf("STARTING TEST\n");

    struct initrd_head_t *head = ptr;
    if(head->magic != HEADMAGIC) {
        printf("Bad head magic\n");
        return;
    }
    printf("offset: %x\n", head->offset);
    struct initrd_file_head_t *initrd_root = ptr + head->offset;
    if(initrd_root->magic != FILEMAGIC) {
        printf("Bad root magic\n");
        return;
    }
    
    for(int i=0;i<initrd_root->size;i++) {
        size_t offset = sizeof(struct initrd_file_head_t) + initrd_root->name_size + i * sizeof(size_t);
        
        struct initrd_file_head_t *entry_one = ptr + *((size_t *)((void *)initrd_root + offset)); // TODO: remove that pointer crazy math
        if(entry_one->magic != FILEMAGIC) {
            printf("Bad entry_one magic: %x\n", entry_one->magic);
            return;
        }
        char *eone_name = kmalloc(entry_one->name_size + 1);
        memcpy(eone_name, (void *)entry_one + sizeof(struct initrd_file_head_t), entry_one->name_size);
        eone_name[entry_one->name_size] = '\0';
        printf("/%s\n", eone_name);
        if(entry_one->type == INITRDFS_DIR) {

            for(int j=0;j<entry_one->size;j++) {
                size_t offset_two = sizeof(struct initrd_file_head_t) + entry_one->name_size + j * sizeof(size_t);
                
                struct initrd_file_head_t *entry_two = ptr + (size_t) *((uint32_t *)((void *)entry_one + offset_two)); // TODO: remove that pointer crazy math
                if(entry_two->magic != FILEMAGIC) {
                    printf("Bad entry_two magic: %x\n", entry_two->magic);
                    return;
                }
                char *etwo_name = kmalloc(entry_two->name_size + 1);
                memcpy(etwo_name, (void *)entry_two + sizeof(struct initrd_file_head_t), entry_two->name_size);
                etwo_name[entry_two->name_size] = '\0';
                printf("/%s/%s ", eone_name, etwo_name);
                kfree(etwo_name);
                if(entry_two->type == INITRDFS_DIR) {
                    printf("wtf is a directory unsupported\n");
                }
                else
                    printf("is file len %x\n", entry_two->size);
            }
        }
        else
            printf("is file len %x\n", entry_one->size);
        kfree(eone_name);
    }
}
