# linker memo

リンカについてのメモ。

もくじ

- gcc -Wl,linker-option
- readelf
- --as-needed、--no-as-needed
- --copy-dt-needed-entries、--no-copy-dt-needed-entries (旧--add-needed、--no-add-needed)
- RUNPATH
- patchelf

## gcc -Wl,linker-option

gccコマンドでリンカーのオプションを指定する場合は``-Wl,linker-option``を使う。
例:

```
gcc -o hello hello.c -Wl,--no-as-needed -lxml2
```

## readelf

実行ファイルでどのシェアードライブラリを必要としているか見るには
``readelf -d executable``とする。``-d``はロングオプションでは``--dynamic``。
NEEDEDのところをみればよい。

例: (実行ファイル)
```
% readelf -d /bin/ls

Dynamic section at offset 0x209d8 contains 29 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libselinux.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libcap.so.2]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x000000000000000c (INIT)               0x3588
 0x000000000000000d (FINI)               0x17214
 0x0000000000000019 (INIT_ARRAY)         0x21ff70
 0x000000000000001b (INIT_ARRAYSZ)       8 (bytes)
 0x000000000000001a (FINI_ARRAY)         0x21ff78
 0x000000000000001c (FINI_ARRAYSZ)       8 (bytes)
 0x000000006ffffef5 (GNU_HASH)           0x330
 0x0000000000000005 (STRTAB)             0x1028
 0x0000000000000006 (SYMTAB)             0x380
 0x000000000000000a (STRSZ)              1538 (bytes)
 0x000000000000000b (SYMENT)             24 (bytes)
 0x0000000000000015 (DEBUG)              0x0
 0x0000000000000003 (PLTGOT)             0x220be8
 0x0000000000000002 (PLTRELSZ)           2688 (bytes)
 0x0000000000000014 (PLTREL)             RELA
 0x0000000000000017 (JMPREL)             0x2b08
 0x0000000000000007 (RELA)               0x17b8
 0x0000000000000008 (RELASZ)             4944 (bytes)
 0x0000000000000009 (RELAENT)            24 (bytes)
 0x0000000000000018 (BIND_NOW)           
 0x000000006ffffffb (FLAGS_1)            Flags: NOW PIE
 0x000000006ffffffe (VERNEED)            0x1738
 0x000000006fffffff (VERNEEDNUM)         1
 0x000000006ffffff0 (VERSYM)             0x162a
 0x000000006ffffff9 (RELACOUNT)          193
 0x0000000000000000 (NULL)               0x0
```

シェアードライブラリファイルも同様:

```
% readelf -d /usr/lib64/libxml2.so

Dynamic section at offset 0x1644a8 contains 33 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libdl.so.2]
 0x0000000000000001 (NEEDED)             Shared library: [libz.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [liblzma.so.5]
 0x0000000000000001 (NEEDED)             Shared library: [libm.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x000000000000000e (SONAME)             Library soname: [libxml2.so.2]
(以下略)
```

## --as-needed, --no-as-needed

## --copy-dt-needed-entries、--no-copy-dt-needed-entries (旧--add-needed、--no-add-needed)

## RUNPATH

## patchelf
