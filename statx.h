/*
 Copyright (C) 2017 Wolfgang Hotwagner <code@feedyourhead.at>       
                                                                
 This file is part of statx-fun                                
 
 This program is free software; you can redistribute it and/or 
 modify it under the terms of the GNU General Public License 
 as published by the Free Software Foundation; either version 2 
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License          
 along with this program; if not, write to the 
 Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, 
 Boston, MA  02110-1301  USA 
*/

#ifndef STATX_H
#define STATX_H

#include <asm-generic/statfs.h>

/* from kernel-sources include/uapi/linux/stat.h */
#define STATX_TYPE              0x00000001U     /* Want/got stx_mode & S_IFMT */
#define STATX_MODE              0x00000002U     /* Want/got stx_mode & ~S_IFMT */
#define STATX_NLINK             0x00000004U     /* Want/got stx_nlink */
#define STATX_UID               0x00000008U     /* Want/got stx_uid */
#define STATX_GID               0x00000010U     /* Want/got stx_gid */
#define STATX_ATIME             0x00000020U     /* Want/got stx_atime */
#define STATX_MTIME             0x00000040U     /* Want/got stx_mtime */
#define STATX_CTIME             0x00000080U     /* Want/got stx_ctime */
#define STATX_INO               0x00000100U     /* Want/got stx_ino */
#define STATX_SIZE              0x00000200U     /* Want/got stx_size */
#define STATX_BLOCKS            0x00000400U     /* Want/got stx_blocks */
#define STATX_BASIC_STATS       0x000007ffU     /* The stuff in the normal stat struct */
#define STATX_BTIME             0x00000800U     /* Want/got stx_btime */
#define STATX_ALL               0x00000fffU     /* All currently supported flags */
#define STATX__RESERVED         0x80000000U     /* Reserved for future struct statx expansion */ 

struct statx_timestamp {
        __s64   tv_sec;
        __u32   tv_nsec;
        __s32   __reserved;
};


struct statx {
        /* 0x00 */
        __u32   stx_mask;       /* What results were written [uncond] */
        __u32   stx_blksize;    /* Preferred general I/O size [uncond] */
        __u64   stx_attributes; /* Flags conveying information about the file [uncond] */
        /* 0x10 */
        __u32   stx_nlink;      /* Number of hard links */
        __u32   stx_uid;        /* User ID of owner */
        __u32   stx_gid;        /* Group ID of owner */
        __u16   stx_mode;       /* File mode */
        __u16   __spare0[1];
        /* 0x20 */
        __u64   stx_ino;        /* Inode number */
        __u64   stx_size;       /* File size */
        __u64   stx_blocks;     /* Number of 512-byte blocks allocated */
        __u64   stx_attributes_mask; /* Mask to show what's supported in stx_attributes */
        /* 0x40 */
        struct statx_timestamp  stx_atime;      /* Last access time */
        struct statx_timestamp  stx_btime;      /* File creation time */
        struct statx_timestamp  stx_ctime;      /* Last attribute change time */
        struct statx_timestamp  stx_mtime;      /* Last data modification time */
        /* 0x80 */
        __u32   stx_rdev_major; /* Device ID of special file [if bdev/cdev] */
        __u32   stx_rdev_minor;
        __u32   stx_dev_major;  /* ID of device containing file [uncond] */
        __u32   stx_dev_minor;
        /* 0x90 */
        __u64   __spare2[14];   /* Spare space for future expansion */
        /* 0x100 */
};


/* from kernel sources: include/uapi/linux/stat.h */
#define AT_STATX_SYNC_TYPE      0x6000  /* Type of synchronisation required from statx() */
#define AT_STATX_SYNC_AS_STAT   0x0000  /* - Do whatever stat() does */
#define AT_STATX_FORCE_SYNC     0x2000  /* - Force the attributes to be sync'd with the server */
#define AT_STATX_DONT_SYNC      0x4000  /* - Don't sync attributes with the server */



#endif 
