/**
 * Finding Filesystems
 * CS 241 - Spring 2019
 */
#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string, "Free blocks: %zd\n"
                            "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    // Thar she blows!
    inode *node = get_inode(fs, path);
    if (!node) {
      errno = ENOENT;
      return -1;
    }
    // problem here
    uint16_t type = node->mode >> RWX_BITS_NUMBER;
    node->mode = new_permissions | (type << RWX_BITS_NUMBER) ;
    clock_gettime(CLOCK_REALTIME, &(node->ctim));
    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    // Land ahoy!
    inode *node = get_inode(fs, path);
    if (!node) {
      errno = ENOENT;
      return -1;
    }
    if (owner != ((uid_t)-1))
      node->uid = owner;
    if (group != ((uid_t)-1))
      node->gid = group;
    clock_gettime(CLOCK_REALTIME, &(node->ctim));
    return 0;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    // Land ahoy!
    if (get_inode(fs, path) || !valid_filename(path)) return NULL;
    const char *filename;
    inode *parent = parent_directory(fs, path, &filename);
    inode_number i_num = first_unused_inode(fs);
    init_inode(parent, &(fs->inode_root[i_num]));
    minixfs_dirent source;
    source.name = strdup(filename);
    source.inode_num = i_num;
    int idx = parent->size/sizeof(data_block);
    size_t offset = parent->size%sizeof(data_block);
    data_block_number d_b_num;
    if (idx < NUM_DIRECT_INODES) {
      d_b_num = parent->direct[idx];
      make_string_from_dirent(fs->data_root[d_b_num].data+offset, source);
    } else {
      d_b_num = parent->indirect;
      data_block_number ind_idx = *((data_block_number *) fs->data_root[d_b_num].data+(idx-NUM_DIRECT_INODES)*sizeof(data_block_number));
      make_string_from_dirent(fs->data_root[ind_idx].data+offset, source);
    }
    free(source.name);
    return fs->inode_root+i_num;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
        char* data_map = GET_DATA_MAP(fs->meta);
        ssize_t num_used_block=0;
        for(uint64_t i=0; i<fs->meta->dblock_count;i++){
          if(data_map[i]==1){
            num_used_block++;
          }
        }
        char* str = block_info_string(num_used_block);
        if(*off > strlen(str)) {
          return 0;
        }
        count = ( count > strlen(str)-*off ? (strlen(str) - *off) : count );
        memmove(buf, str+*off, count);
        *off+=count;
        return count;
    }
    // TODO implement your own virtual file here
    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {
    // X marks the spot
    printf("pass minixfs_write\n");
    if (count+*off > (NUM_DIRECT_INODES+NUM_INDIRECT_INODES)*sizeof(data_block)) {
      errno = ENOSPC;
      return -1;
    }
    inode *node = get_inode(fs, path);
    if (!node) {
      node = minixfs_create_inode_for_path(fs, path);
    }
    int block_count = (count+*off)/sizeof(data_block)+1;
    if (minixfs_min_blockcount(fs, path, block_count) == -1) {
      errno = ENOSPC;
      return -1;
    }
    int idx = *off/sizeof(data_block);
    int offset = *off%sizeof(data_block);
    data_block_number num;
    size_t bytes_write = 0;
    while (bytes_write < count) {
      if (idx < NUM_DIRECT_INODES) {
        num = node->direct[idx];
      } else {
        num = *((data_block_number *) fs->data_root[node->indirect].data+(idx-NUM_DIRECT_INODES)*sizeof(data_block_number));
      }
      size_t ret = (count-bytes_write)>(sizeof(data_block)-offset)?(sizeof(data_block)-offset):(count-bytes_write);
      memcpy(fs->data_root[num].data+offset, buf+bytes_write, ret);
      bytes_write += ret;
      offset = 0;
      idx++;
    }
    *off += count;
    node->size = *off;
    clock_gettime(CLOCK_REALTIME, &(node->mtim));
    clock_gettime(CLOCK_REALTIME, &(node->atim));
    return count;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);
    // 'ere be treasure!
    inode *node = get_inode(fs, path);
    if (!node) {
      errno = ENOENT;
      return -1;
    }
    int idx = *off/sizeof(data_block);
    int offset = *off%sizeof(data_block);
    data_block_number num;
    size_t bytes_read = 0;
    count = count > node->size-*off ? node->size-*off : count;
    while (bytes_read < count) {
      if (idx < NUM_DIRECT_INODES) {
        num = node->direct[idx];
      } else {
        num = *((data_block_number *) fs->data_root[node->indirect].data+(idx-NUM_DIRECT_INODES)*sizeof(data_block_number));
      }
      size_t ret = (count-bytes_read)>(sizeof(data_block)-offset)?(sizeof(data_block)-offset):(count-bytes_read);
      memcpy(buf+bytes_read, fs->data_root[num].data+offset, ret);
      bytes_read += ret;
      offset = 0;
      idx++;
    }
    *off += bytes_read;
    clock_gettime(CLOCK_REALTIME, &(node->atim));
    return bytes_read;
}
