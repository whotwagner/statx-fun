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

#define _GNU_SOURCE
#define _ATFILE_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include "statx.h"
#include <time.h>
#include <getopt.h>
#include <string.h>

#define VERSION 1.0.1

/*
   does not (yet) provide a wrapper for the statx() system call 
*/
#include <sys/syscall.h>

/* this code works ony with x86, arm64 and x86_64 */
#if __x86_64__
#define __NR_statx 332
#else
  #if __aarch64__
    #define __NR_statx 291
  #else
    #define __NR_statx 383
  #endif
#endif

#define statx(a,b,c,d,e) syscall(__NR_statx,(a),(b),(c),(d),(e))

/* from kernel-sources(samples/statx/test-statx.c) */
static void print_time(const char *field, struct statx_timestamp *ts)
{
        struct tm tm;
        time_t tim;
        char buffer[100];
        int len;

        tim = ts->tv_sec;
        if (!localtime_r(&tim, &tm)) {
                perror("localtime_r");
                exit(1);
        }
        len = strftime(buffer, 100, "%F %T", &tm);
        if (len == 0) {
                perror("strftime");
                exit(1);
        }
        printf("%s", field);
        fwrite(buffer, 1, len, stdout);
        printf(".%09u", ts->tv_nsec);
        len = strftime(buffer, 100, "%z", &tm);
        if (len == 0) {
                perror("strftime2");
                exit(1);
        }
        fwrite(buffer, 1, len, stdout);
        printf("\n");
}

/* from kernel-sources(samples/statx/test-statx.c) */
static void dump_statx(struct statx *stx)
{
	char buffer[256], ft = '?';

	printf("results=%x\n", stx->stx_mask);

	printf(" ");
	if (stx->stx_mask & STATX_SIZE)
		printf(" Size: %-15llu", (unsigned long long)stx->stx_size);
	if (stx->stx_mask & STATX_BLOCKS)
		printf(" Blocks: %-10llu", (unsigned long long)stx->stx_blocks);
	printf(" IO Block: %-6llu", (unsigned long long)stx->stx_blksize);
	if (stx->stx_mask & STATX_TYPE) {
		switch (stx->stx_mode & S_IFMT) {
		case S_IFIFO:	printf("  FIFO\n");			ft = 'p'; break;
		case S_IFCHR:	printf("  character special file\n");	ft = 'c'; break;
		case S_IFDIR:	printf("  directory\n");		ft = 'd'; break;
		case S_IFBLK:	printf("  block special file\n");	ft = 'b'; break;
		case S_IFREG:	printf("  regular file\n");		ft = '-'; break;
		case S_IFLNK:	printf("  symbolic link\n");		ft = 'l'; break;
		case S_IFSOCK:	printf("  socket\n");			ft = 's'; break;
		default:
			printf(" unknown type (%o)\n", stx->stx_mode & S_IFMT);
			break;
		}
	} else {
		printf(" no type\n");
	}

	sprintf(buffer, "%02x:%02x", stx->stx_dev_major, stx->stx_dev_minor);
	printf("Device: %-15s", buffer);
	if (stx->stx_mask & STATX_INO)
		printf(" Inode: %-11llu", (unsigned long long) stx->stx_ino);
	if (stx->stx_mask & STATX_NLINK)
		printf(" Links: %-5u", stx->stx_nlink);
	if (stx->stx_mask & STATX_TYPE) {
		switch (stx->stx_mode & S_IFMT) {
		case S_IFBLK:
		case S_IFCHR:
			printf(" Device type: %u,%u",
			       stx->stx_rdev_major, stx->stx_rdev_minor);
			break;
		}
	}
	printf("\n");

	if (stx->stx_mask & STATX_MODE)
		printf("Access: (%04o/%c%c%c%c%c%c%c%c%c%c)  ",
		       stx->stx_mode & 07777,
		       ft,
		       stx->stx_mode & S_IRUSR ? 'r' : '-',
		       stx->stx_mode & S_IWUSR ? 'w' : '-',
		       stx->stx_mode & S_IXUSR ? 'x' : '-',
		       stx->stx_mode & S_IRGRP ? 'r' : '-',
		       stx->stx_mode & S_IWGRP ? 'w' : '-',
		       stx->stx_mode & S_IXGRP ? 'x' : '-',
		       stx->stx_mode & S_IROTH ? 'r' : '-',
		       stx->stx_mode & S_IWOTH ? 'w' : '-',
		       stx->stx_mode & S_IXOTH ? 'x' : '-');
	if (stx->stx_mask & STATX_UID)
		printf("Uid: %5d   ", stx->stx_uid);
	if (stx->stx_mask & STATX_GID)
		printf("Gid: %5d\n", stx->stx_gid);

	if (stx->stx_mask & STATX_ATIME)
		print_time("Access: ", &stx->stx_atime);
	if (stx->stx_mask & STATX_MTIME)
		print_time("Modify: ", &stx->stx_mtime);
	if (stx->stx_mask & STATX_CTIME)
		print_time("Change: ", &stx->stx_ctime);
	if (stx->stx_mask & STATX_BTIME)
		print_time(" Birth: ", &stx->stx_btime);

	if (stx->stx_attributes_mask) {
		unsigned char bits, mbits;
		int loop, byte;

		static char attr_representation[64 + 1] =
			/* STATX_ATTR_ flags: */
			"????????"	/* 63-56 */
			"????????"	/* 55-48 */
			"????????"	/* 47-40 */
			"????????"	/* 39-32 */
			"????????"	/* 31-24	0x00000000-ff000000 */
			"????????"	/* 23-16	0x00000000-00ff0000 */
			"???me???"	/* 15- 8	0x00000000-0000ff00 */
			"?dai?c??"	/*  7- 0	0x00000000-000000ff */
			;

		printf("Attributes: %016llx (", stx->stx_attributes);
		for (byte = 64 - 8; byte >= 0; byte -= 8) {
			bits = stx->stx_attributes >> byte;
			mbits = stx->stx_attributes_mask >> byte;
			for (loop = 7; loop >= 0; loop--) {
				int bit = byte + loop;

				if (!(mbits & 0x80))
					putchar('.');	/* Not supported */
				else if (bits & 0x80)
					putchar(attr_representation[63 - bit]);
				else
					putchar('-');	/* Not set */
				bits <<= 1;
				mbits <<= 1;
			}
			if (byte)
				putchar(' ');
		}
		printf(")\n");
	}
}

void print_help(const char *progname)
{
	printf("usage: %s [OPTION].. [FILE]...\n",progname); 
	printf("\noptions:\n");
	printf("\t-a.....no automount\n");
	printf("\t-l.....dereference links\n");
	printf("\t-b.....basic stats\n");
	printf("\t-s.....get filesize only\n");
	printf("\t-f.....force sync(remote fs)\n");
	printf("\t-d.....don't sync(remote fs)\n");
	printf("\t-h.....help\n");
}

int main(int argc, char *argv[])
{
	int dirfd = AT_FDCWD;
	int flags = AT_SYMLINK_NOFOLLOW;
	unsigned int mask = STATX_ALL;
	struct statx stxbuf;
	long ret = 0;
	int i = 0;
	int opt = 0;

	while(( opt = getopt(argc, argv, "halbsdf")) != -1)
	{
		switch(opt) {
			case 'h':
				print_help(argv[0]);
				exit(EXIT_SUCCESS);
				break;
			case 'a':
				flags |= AT_NO_AUTOMOUNT;
				break;
			case 'l':
				flags &= ~AT_SYMLINK_NOFOLLOW;
				break;
			case 'b':
				mask &= ~STATX_BASIC_STATS;
				break;
			case 's':
				mask = STATX_SIZE;
				break;
			case 'f':
				flags &= ~AT_STATX_SYNC_TYPE;
				flags |= AT_STATX_FORCE_SYNC;
				break;
			case 'd':
				flags &= ~AT_STATX_SYNC_TYPE;
				flags |= AT_STATX_DONT_SYNC;
				break;
			default:
				print_help(argv[0]);
				exit(EXIT_SUCCESS);
				break;
		}
	}

	if (optind >= argc) {
		print_help(argv[0]);
		exit(EXIT_FAILURE);
	}

  	for (i = optind; i < argc; i++)
	{
		printf("Filename: %s\n",argv[i]);
		printf("Mask: %x\n",mask);

		memset(&stxbuf, 0xbf, sizeof(stxbuf));
		ret = statx(dirfd,argv[i],flags,mask,&stxbuf);
		if( ret < 0)
		{
			perror("statx");
			return EXIT_FAILURE;
		}
		dump_statx(&stxbuf);
		printf("\n");
	}
	return EXIT_SUCCESS;
}
