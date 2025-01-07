# patchelfでRUNPATHを追加

patchelfでRUNPATHを追加してみる例題。

mylib.cがライブラリファイル。
main.cがmain関数があるプログラム。

```
(シェアードライブラリファイルを作成)
% cc -o libmylib.so -fPIC -shared mylib.c

(fileコマンドで見てみる)
% file libmylib.so
libmylib.so: ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked, BuildID[sha1]=6b45037505ac1d9b1d13354511fd705c11c42639, not stripped

(mylibを使うプログラムをコンパイル)
% cc -o main main.c -L. -lmylib

(実行してみる)
% ./main
./main: error while loading shared libraries: libmylib.so: cannot open shared object file: No such file or directory
(libmylib.soがみつからない)

(readelfでRUNPATHが入っているか確認してみる)
% readelf -d main

Dynamic section at offset 0x2e40 contains 21 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libmylib.so]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x000000000000000c (INIT)               0x401000
 0x000000000000000d (FINI)               0x401118
 0x0000000000000019 (INIT_ARRAY)         0x403e30
 0x000000000000001b (INIT_ARRAYSZ)       8 (bytes)
 0x000000000000001a (FINI_ARRAY)         0x403e38
 0x000000000000001c (FINI_ARRAYSZ)       8 (bytes)
 0x000000006ffffef5 (GNU_HASH)           0x4003a0
 0x0000000000000005 (STRTAB)             0x400438
 0x0000000000000006 (SYMTAB)             0x4003c0
 0x000000000000000a (STRSZ)              121 (bytes)
 0x000000000000000b (SYMENT)             24 (bytes)
 0x0000000000000015 (DEBUG)              0x0
 0x0000000000000007 (RELA)               0x4004e0
 0x0000000000000008 (RELASZ)             96 (bytes)
 0x0000000000000009 (RELAENT)            24 (bytes)
 0x000000006ffffffe (VERNEED)            0x4004c0
 0x000000006fffffff (VERNEEDNUM)         1
 0x000000006ffffff0 (VERSYM)             0x4004b2
 0x0000000000000000 (NULL)               0x0
(RUNPATHが入っていない)

(dnfでpatchelfをセット)
# dnf install patchelf

(patchelfでmainにRUNPATHを入れる)
% patchelf --set-rpath '$ORIGIN' main

(readelfでRUNPATHが入ったかどうかみる)
% readelf -d main

Dynamic section at offset 0x388 contains 26 entries:
  Tag        Type                         Name/Value
 0x000000000000001d (RUNPATH)            Library runpath: [$ORIGIN]
 0x0000000000000001 (NEEDED)             Shared library: [libmylib.so]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x000000000000000c (INIT)               0x401000
 0x000000000000000d (FINI)               0x401148
 0x0000000000000019 (INIT_ARRAY)         0x403df0
 0x000000000000001b (INIT_ARRAYSZ)       8 (bytes)
 0x000000000000001a (FINI_ARRAY)         0x403df8
 0x000000000000001c (FINI_ARRAYSZ)       8 (bytes)
 0x000000006ffffef5 (GNU_HASH)           0x3ff698
 0x0000000000000005 (STRTAB)             0x3ff578
 0x0000000000000006 (SYMTAB)             0x3ff608
 0x000000000000000a (STRSZ)              138 (bytes)
 0x000000000000000b (SYMENT)             24 (bytes)
 0x0000000000000015 (DEBUG)              0x0
 0x0000000000000003 (PLTGOT)             0x404000
 0x0000000000000002 (PLTRELSZ)           24 (bytes)
 0x0000000000000014 (PLTREL)             RELA
 0x0000000000000017 (JMPREL)             0x400560
 0x0000000000000007 (RELA)               0x400500
 0x0000000000000008 (RELASZ)             96 (bytes)
 0x0000000000000009 (RELAENT)            24 (bytes)
 0x000000006ffffffe (VERNEED)            0x4004e0
 0x000000006fffffff (VERNEEDNUM)         1
 0x000000006ffffff0 (VERSYM)             0x4004d2
 0x0000000000000000 (NULL)               0x0

(lddの出力)
% ldd main
	linux-vdso.so.1 (0x00007ffe3149d000)
	libmylib.so => /home/myname/prog/mylib/./libmylib.so (0x00007f6af06da000)
	libc.so.6 => /lib64/libc.so.6 (0x00007f6af0400000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f6af06e1000)

(実行してみる)
% ./main
my_print() in mylib.c
```
