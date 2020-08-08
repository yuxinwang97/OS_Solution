
#include <stdlib.h>
#include "ext2_fs.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

int fs = -1;//the file descripter to the file image we use
struct ext2_super_block superblock;
struct ext2_group_desc group;
struct ext2_inode inode;
struct ext2_dir_entry dir;

void directory_entries(uint32_t offset, int parent, uint32_t blocksize) {
    uint32_t current = offset;
    while(current < offset + blocksize) {
        if(pread(fs, &dir, sizeof(struct ext2_dir_entry), current) <0) {
            fprintf(stderr, "Error: fail to pread to dir\n");
        }
        if(dir.inode != 0) {
            printf("DIRENT,%d,%d,%d,%d,%d,'%s'\n", parent, (current-offset), dir.inode, dir.rec_len, dir.name_len, dir.name);
        }
        current += dir.rec_len;
    }
}


void entry(int offset, int parent, uint32_t blocksize) {
    uint32_t i, j, k;
    uint32_t temp, temp1, temp2, temp3;

    //level 0
    for(i = 0; i < 12; i++) {
        if(pread(fs, &temp, sizeof(uint32_t), offset+40+4*i) < 0) {
            fprintf(stderr, "Error: fail to pread in entry0\n");
            exit(1);
        }
        if(temp != 0) {
            directory_entries(temp*blocksize, parent, blocksize);
        }
    }
    temp = 0;

    //level 1
    if(pread(fs, &temp1, sizeof(uint32_t), offset+40+12*sizeof(uint32_t)) < 0) {
        fprintf(stderr, "Error: fail to pread in entry1\n");
        exit(1);
    }
    if(temp1 != 0) {
        for(i = 0; i < blocksize/4; i++) {
            if(pread(fs, &temp, sizeof(uint32_t), blocksize*temp1+4*i) < 0) {
                fprintf(stderr, "Error: fail to pread in entry0\n");
                exit(1);
            }
            if(temp != 0) {
                directory_entries(temp*blocksize, parent, blocksize);
            }
        }
    }
    temp = 0;
    temp1 = 0;

    //level 2
    if(pread(fs, &temp2, sizeof(uint32_t), offset+40+13*sizeof(uint32_t)) < 0){
        fprintf(stderr, "Error: fail to pread in entry0\n");
        exit(1);
    }
    if(temp2 != 0) {
        for(i = 0; i < blocksize/4; i++) {
            if(pread(fs, &temp1, sizeof(uint32_t), blocksize*temp2+4*i) < 0) {
                fprintf(stderr, "Error: fail to pread in entry0\n");
                exit(1);
            }
            if(temp1 != 0) {
                for(j = 0; j < blocksize/4; j++) {
                    if(pread(fs, &temp, sizeof(uint32_t), blocksize*temp1+4*j) < 0) {
                        fprintf(stderr, "Error: fail to pread in entry0\n");
                        exit(1);
                    }
                    if(temp != 0) {
                        directory_entries(temp*blocksize, parent, blocksize);
                    }
                }
            }
        }
    }
    temp = 0;
    temp1 = 0;
    temp2 = 0;

    //level 3
    if(pread(fs, &temp3, sizeof(uint32_t), offset+40+14*sizeof(uint32_t)) < 0) {
        fprintf(stderr, "Error: fail to pread in entry3\n");
        exit(1);
    }
    if(temp3 != 0) {
        for(i = 0; i < blocksize/4; i++) {
            if(pread(fs, &temp2, sizeof(uint32_t), blocksize*temp3+4*i) < 0) {
                fprintf(stderr, "Error: fail to pread in entry2\n");
                exit(1);
            }
            if(temp2 != 0) {
                for(j = 0; j < blocksize/4; j++) {
                    if(pread(fs, &temp1, sizeof(uint32_t), blocksize*temp2+4*j) < 0) {
                        fprintf(stderr, "Error: fail to pread in entry1\n");
                        exit(1);
                    }
                    if(temp1 != 0) {
                        for(k = 0; k < blocksize/4; k++) {
                            if(pread(fs, &temp, sizeof(uint32_t), blocksize*temp1+4*k) < 0) {
                                fprintf(stderr, "Error: fail to pread in entry0\n");
                                exit(1);
                            }
                            if(temp != 0) {
                                directory_entries(temp*blocksize, parent, blocksize);
                            }   
                        }
                    }
                }
            }
        }
    }


}


void deal_level1(uint32_t inode_num, uint32_t block_num, uint32_t blocksize){
    uint32_t temp;
    uint32_t k;
    for (k = 0; k < blocksize/4; k++){
        if(pread(fs, &temp, sizeof(uint32_t), block_num*blocksize + k*4) < 0) {
            fprintf(stderr, "fail to read level 1");
            exit(2);
        }
        if (temp != 0){
            printf("INDIRECT,%u,1,%u,%u,%u\n",inode_num+1,k+12,block_num,temp);
        }
    }
}

void deal_level2(uint32_t inode_num, uint32_t block_num, uint32_t blocksize){
    uint32_t temp;
    uint32_t temp_2;
    uint32_t k;
    uint32_t l;
    for (k = 0; k < blocksize/4; k++){
        if(pread(fs, &temp, sizeof(uint32_t), block_num*blocksize + k*4) < 0) {
            fprintf(stderr, "fail to read level 2");
            exit(2);
        }
        if(temp != 0){
            printf("INDIRECT,%u,2,%u,%u,%u\n",inode_num+1,(k+1)*blocksize/4+ 12,block_num,temp);
            for (l = 0; l < blocksize/4; l++){
                if(pread(fs, &temp_2, sizeof(uint32_t), temp*blocksize + l*4) < 0) {
                    fprintf(stderr, "fail to read level 1 from level 2");
                    exit(2);
                }
                if (temp_2 != 0){
                    printf("INDIRECT,%u,1,%u,%u,%u\n",inode_num+1,(k+1)*blocksize/4+ 12+l,temp,temp_2);
                }
            }
        }
    }
}

void deal_level3(uint32_t inode_num, uint32_t block_num, uint32_t blocksize){
    uint32_t temp;
    uint32_t temp_2;
    uint32_t temp_3;
    uint32_t k;
    uint32_t l;
    uint32_t m;
    uint32_t off = (blocksize/4) * (blocksize/4 + 1)+12;
    for (k = 0; k < blocksize/4; k++){
        if(pread(fs, &temp, sizeof(uint32_t), block_num*blocksize + k*4) < 0) {
            fprintf(stderr, "fail to read level 2");
            exit(2);
        }
        if(temp != 0){
            printf("INDIRECT,%u,3,%u,%u,%u\n",inode_num+1,off + k*blocksize*blocksize/8,block_num,temp);
            for (l = 0; l < blocksize/4; l++){
                if(pread(fs, &temp_2, sizeof(uint32_t), temp*blocksize + l*4) < 0) {
                    fprintf(stderr, "fail to read level 1 from level 2");
                    exit(2);
                }
                if (temp_2 != 0){
                    printf("INDIRECT,%u,2,%u,%u,%u\n",inode_num+1,off + k*blocksize*blocksize/8 + l*blocksize/4,temp,temp_2);
                    for (m = 0; m < blocksize/4; m++){
                        if(pread(fs, &temp_3, sizeof(uint32_t), temp_2*blocksize + m*4) < 0) {
                            fprintf(stderr, "fail to read level 1 from level 2");
                            exit(2);
                        }   
                        if (temp_3 != 0){
                            printf("INDIRECT,%u,1,%u,%u,%u\n",inode_num+1,off + k*blocksize*blocksize/8 + l*blocksize/4 +m,temp_2,temp_3);
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    if(argc != 2) {
        fprintf(stderr, "Error: Incorrect number of argument\n");
        exit(1);
    }
    if((fs = open(argv[1], O_RDONLY)) == -1) {
        fprintf(stderr, "Error: Fail to mount to the image\n" );
        exit(2);
    }


    //superblock summary
    if(pread(fs, &superblock, sizeof(struct ext2_super_block), 1024) < 0) {
        fprintf(stderr, "Error: Fail to pread superblock\n");
        exit(2);
    }
    uint32_t blockcount = superblock.s_blocks_count;
    uint32_t inodecount = superblock.s_inodes_count;
    uint32_t blocksize = EXT2_MIN_BLOCK_SIZE << superblock.s_log_block_size;
    uint32_t inodesize = superblock.s_inode_size;
    uint32_t blockpergroup = superblock.s_blocks_per_group;
    uint32_t inodepergroup = superblock.s_inodes_per_group;
    uint32_t firstinode = superblock.s_first_ino;
    printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", blockcount, inodecount, blocksize, inodesize, blockpergroup, inodepergroup, firstinode);

    //block group summary
    if(pread(fs, &group, sizeof(struct ext2_group_desc), 1024+sizeof(struct ext2_super_block)) < 0) {
        fprintf(stderr, "Error: Fail to pread block group\n");
        exit(2);
    }
    uint16_t freeblockcount = group.bg_free_blocks_count;
    uint16_t freeinodecount = group.bg_free_inodes_count;
    uint32_t blockbitmap = group.bg_block_bitmap;
    uint32_t inodebitmap = group.bg_inode_bitmap;
    uint32_t inodetable = group.bg_inode_table;
    //blockpergroup->blockcount
    printf("GROUP,0,%d,%d,%d,%d,%d,%d,%d\n", blockcount, inodepergroup, freeblockcount, freeinodecount, blockbitmap, inodebitmap, inodetable);
    
    uint32_t i;

    //free block entries
    unsigned char eachblock;
    for (i = 0; i < blockcount; i++){
        if(pread(fs, &eachblock, sizeof(uint8_t), blockbitmap*blocksize+(i >> 3)) < 0) {
            fprintf(stderr, "Error: Fail to pread from bitmap to each block\n");
            exit(2);
        }
        if ( !(eachblock & (1 << (i&7) ) ) ){
            printf("BFREE,%d\n", i+1);
        }
    }

   
    //free inode summary
    unsigned char eachinode;
    for (i = 0; i < inodecount; i++){
        if(pread(fs, &eachinode, sizeof(uint8_t), inodebitmap*blocksize+(i >> 3)) < 0) {
            fprintf(stderr, "Error: Fail to pread from bitmap to each inode\n");
            exit(2);
        }
        if ( (eachinode & (1 << (i&7) )) == 0 ){
            printf("IFREE,%d\n", i+1);
        }
    }
    

    //usd inode summary
    for(i = 0; i < inodecount; i++) {
        if(pread(fs, &eachinode, sizeof(uint8_t), inodebitmap*blocksize+(i >> 3)) < 0) {
            fprintf(stderr, "Error: Fail to pread from bitmap\n");
            exit(2);
        }
        if ( (eachinode & (1 << (i&7) ) ) != 0 ){
            //fprintf(stderr,"Laowang3\n");
            if(pread(fs, &inode, inodesize, 1024+(inodetable-1)*(blocksize)+sizeof(struct ext2_inode)*i) < 0) {
                fprintf(stderr, "Error: Fail to pread to inode\n");
                exit(2);
            }
            //fprintf(stderr,"mode:%u,link:%u\n",inode.i_mode,inode.i_links_count);
            if (inode.i_mode == 0 || inode.i_links_count == 0){
                continue;
            }

            char type = '?';
            if (S_ISDIR(inode.i_mode)) {
                type = 'd';
            }
            else if (S_ISREG(inode.i_mode)) type = 'f';
            else if (S_ISLNK(inode.i_mode)) type = 's';

            //last inode change time, modification time and last access time(mm/dd/yy hh:mm:ss, GMT)
            char atime_array[32];
            char ctime_array[32];
            char mtime_array[32];
            time_t atime = inode.i_atime;
            time_t ctime = inode.i_ctime;
            time_t mtime = inode.i_mtime;

            struct tm* atime_time = gmtime(&atime);
            if(atime_time == NULL) {
                fprintf(stderr,"fail to get access time\n");
                exit(1);
            }
            sprintf(atime_array, "%02d/%02d/%02d %02d:%02d:%02d", 
                    (atime_time->tm_mon) +1, 
                    atime_time->tm_mday, 
                    (atime_time->tm_year) %100, 
                    atime_time->tm_hour, 
                    atime_time->tm_min, 
                    atime_time->tm_sec);
            struct tm* ctime_time = gmtime(&ctime);
            if(ctime_time == NULL) {
                fprintf(stderr,"fail to get inode change time\n");
                exit(1);
            }
            sprintf(ctime_array, "%02d/%02d/%02d %02d:%02d:%02d", 
                    (ctime_time->tm_mon) +1, 
                    ctime_time->tm_mday, 
                    (ctime_time->tm_year) %100, 
                    ctime_time->tm_hour, 
                    ctime_time->tm_min, 
                    ctime_time->tm_sec);
            struct tm* mtime_time = gmtime(&mtime);
            if(mtime_time == NULL) {
                fprintf(stderr,"fail to get modify time\n");
                exit(1);
            }
            sprintf(mtime_array, "%02d/%02d/%02d %02d:%02d:%02d", 
                    (mtime_time->tm_mon) +1, 
                    mtime_time->tm_mday, 
                    (mtime_time->tm_year) %100, 
                    mtime_time->tm_hour, 
                    mtime_time->tm_min, 
                    mtime_time->tm_sec);
            
            
            printf("INODE,%d,%c,%o,%d,%d,%d,%s,%s,%s,%d,%d", 
            i+1, type, inode.i_mode&0xfff, inode.i_uid, inode.i_gid, inode.i_links_count, ctime_array, mtime_array, atime_array, inode.i_size, inode.i_blocks);

            if (type == 's' && inode.i_size <= 60) { printf(",%u\n", inode.i_block[0]); }
            else {
                uint32_t j;
                for (j = 0; j < 15; j++){
                    printf(",%u", inode.i_block[j]);
                }
                printf("\n");
            }

            if(type == 'd') {
                int offset = 1024+(inodetable-1)*(blocksize)+sizeof(struct ext2_inode)*i;
                int parent = i+1;
                entry(offset, parent, blocksize);
            }

            if (type != 's'){
                //deal_level1(uint32_t inode_num, uint32_t block_num, uint32_t blocksize)
                if (inode.i_block[12] != 0) deal_level1(i,inode.i_block[12],blocksize);
                if (inode.i_block[13] != 0) deal_level2(i,inode.i_block[13],blocksize);
                if (inode.i_block[14] != 0) deal_level3(i,inode.i_block[14],blocksize);
            }

        }
    }


    return 0;
}