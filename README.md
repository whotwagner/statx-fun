# statx

This project is an example implementation of the statx()-systemcall which was introduced in Linux 4.11

## Requirements

statx-fun was written for x86, arm64 and x86_64 architectures. but it just takes two lines of code(place the right syscall-number) to port it to other architectures. It requires at least Linux Kernel 4.11.

Please note, that at the moment there is no glibc wrapper for statx()-systemcall.

## Compile

```
make
```

## Usage

### Basic Usage

```
./statx ./statx.c
Filename: statx.c
Mask: fff
results=fff
  Size: 7313            Blocks: 16         IO Block: 4096    regular file
Device: fe:02           Inode: 10227350    Links: 1    
Access: (0644/-rw-r--r--)  Uid:  1000   Gid:  1000
Access: 2017-06-02 09:51:32.982941917+0200
Modify: 2017-06-02 09:48:16.313825618+0200
Change: 2017-06-02 09:48:16.321825666+0200
 Birth: 2017-06-02 09:48:16.313825618+0200
Attributes: 0000000000000000 (........ ........ ........ ........ ........ ........ ....-... .---.-..)

```
### Help

```
./statx
usage: ./statx [OPTION]... [FILE]...

options:
	-a.....no automount
	-l.....dereference links
	-b.....basic stats
	-s.....get filesize only
	-f.....force sync(remote fs)
	-d.....don't sync(remote fs)
	-h.....help
```
## Documentation
   * [Man-Page](https://www.mankier.com/2/statx)
   * Kernel 4.11 Source-Tree (see samples/statx/test-statx.c)

## Licence

GPL

## Author Information

Wolfgang Hotwagner [**(https://tech.feedyourhead.at/)**](https://tech.feedyourhead.at)
