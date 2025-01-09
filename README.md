# linker memo

リンカについてのメモ。

もくじ

- man 5 elf
- gcc -Wl,linker-option
- readelf
- --as-needed、--no-as-needed
- --copy-dt-needed-entries、--no-copy-dt-needed-entries (旧--add-needed、--no-add-needed)
- RPATH、RUNPATH
- patchelf

## ELFフォーマットマニュアルページ

- [ELFフォーマットマニュアルページ](https://man7.org/linux/man-pages/man5/elf.5.html)
- [Linuxダイナミックリンカ/ローダ (ld.so) マニュアルページ](https://man7.org/linux/man-pages/man8/ld.so.8.html)

ld.soマニュアルページにはelfファイル内のDT_RPATH、DT_RUNPATH、環境変数LD_LIBRARY_PATHの
優先順位が書いてある。

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

``--as-needed``が指定されるとそれ以降に指定されたシェアードライブラリが
必要かどうか判断され、必要なら``DT_NEEDED``にライブラリ名が入り、
必要なければ入らない。

``--no-as-needed``が指定されるとそれ以降に指定されたシェアードライブラリ
が必ず``DT_NEEDED``に入る。

どちらがデフォルトであるかはLinuxディストリビューションで違い
- debian, ubuntuでは``--as-needed``
- AlmaLinux, ArchLinuxでは``--no-as-needed``
がデフォルトになっている。

例題:

たとえばsample.cとして
```
int main(int argc, char *argv[])
{
    return 0;
}
```
というプログラムを用意して

```
cc -o sample sample.c -lxml2
```
とコンパイルして、``readelf -d sample``で見るとNEEDEDはlibc.so.6だけである。

AlmaLinux 8, 9で同様なことをすると
```
 0x0000000000000001 (NEEDED)             Shared library: [libxml2.so.2]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
```
になっている。

上のプログラムを``cc -v -g -O2 -Wall -o sample sample``でコンパイルした
ときのcollect2のオプションを抜き出したもの(collect2の行を抜き出し
``sed -e 's/ /\n/g'``したもの)。

- [AlmaLinux 9](ld.almalinux.txt)
- [ubuntu 202404LTS](ld.ubuntu.txt)
- 差分 ``diff -u ld.almalinux.txt ld.ubuntu.txt``の結果 [diff.linker-option.txt](diff.linker-option.txt)

## --copy-dt-needed-entries、--no-copy-dt-needed-entries (旧--add-needed、--no-add-needed)

``--add-needed``はdeprecatedで、``--copy-dt-needed-entries``を使う。
``--no-add-needed``もdeprecatedで``--no-copy-dt-needed-entries``を使う。

AlmaLinuxではリンカが``--no-add-needed``付きで起動されている(``--no-copy-dt-needed-entries``の動作をする)。
Debian, Ubuntuでは指定されていないが、AlmaLinux同様に``--no-copy-dt-needed-entries``がデフォルトに
なっているようだ。

たとえばlibxml2は
```
% readelf -d /usr/lib64/libxml2.so

Dynamic section at offset 0x1644a8 contains 33 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libdl.so.2]
 0x0000000000000001 (NEEDED)             Shared library: [libz.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [liblzma.so.5]
 0x0000000000000001 (NEEDED)             Shared library: [libm.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
```
のようにlibz, liblzma, libmとlibcがNEEDEDになっている。
[lxml2-example/lxml2-example.c](lxml2-example/lxml2-example.c)
のようにlibz関数のみを使うプログラムをコンパイルするときに
```
cc -g -O2 -Wall -std=gnu17  -Wl,--copy-dt-needed-entries  lxml2-example.c  -lxml2 -o lxml2-example
```
と``-lz``なしにコンパイルしてもエラーにならずに正常に実行ファイルができる。
できあがったlxml2-example実行ファイルは

```
% readelf -d lxml2-example

Dynamic section at offset 0xdf0 contains 26 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libxml2.so.2]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libz.so.1]
```

のようにlibzが埋め込まれるようになる。

現在は上に書いたように``--no-copy-dt-needed-entries``がデフォルトになっている。

## RPATH、RUNPATH

実行ファイル、あるいはシェアードライブラリファイルにシェアードライブラリ
ファイルの検索パスを埋め込むことができる(``DT_RPATH``あるいは``DT_RUNPATH``)。
``$ORIGION``とかけばそのファイルが存在したディレクトリに展開される。
``$ORIGIN:$ORIGIN/../lib``として複数の値を指定することもできる。

シェルのコマンドラインから``$ORIGIN``を埋め込む際には、シェル変数として展開
されないように``'`` (single quote)で囲む。
```
gcc ... -Wl,-rpath,'$ORIGIN'
```
Makefileでは``'$$ORIGIN'``と書く。

``gcc ... -Wl,-rpath,mypath``と指定した場合、AlmaLinux 8, 9, Kittenでは
``RPATH``に格納される。Debian, Ubuntu, ArchLinuxでは``RUNPATH``に格納される。
RUNPATHのほうが推奨されているようだ。

AlmaLinuxで``RUNPATH``に埋め込むには``--enable-new-dtags``オプションを指定する:
```
gcc ... -Wl,--enable-new-dtags -Wl,-rpath,mypath
```

## patchelf

patchelfでELF (Executable and Linkable Format)を編集できる。
たとえばあとからRUNPATHを追加することができる。
[例題](patchelf-example)

## ROOT

AlmaLinuxにepelリポジトリからいれたrootについてはライブラリファイルに
RUNPATHはついていない:
```
% readelf -d libCore.so

Dynamic section at offset 0x46a448 contains 38 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libpcre2-8.so.0]
 0x0000000000000001 (NEEDED)             Shared library: [libz.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [liblzma.so.5]
 0x0000000000000001 (NEEDED)             Shared library: [libxxhash.so.0]
 0x0000000000000001 (NEEDED)             Shared library: [liblz4.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libzstd.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libstdc++.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libm.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libgcc_s.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [ld-linux-x86-64.so.2]
 0x000000000000000e (SONAME)             Library soname: [libCore.so.6.32]
(以下略) 
```

ソースからコンパイルするとRUNPATHが入っている:
```
% readelf -d libCore.so

Dynamic section at offset 0x597d40 contains 32 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libz.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libstdc++.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libm.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libgcc_s.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [ld-linux-x86-64.so.2]
 0x000000000000000e (SONAME)             Library soname: [libCore.so]
 0x000000000000001d (RUNPATH)            Library runpath: [$ORIGIN:$ORIGIN/../lib]
```

https://root.cern/ からダウンロードしたubuntu用pre-compiled binaryは
RUNPATHが入っている:

```
% readelf -d /usr/local/root/lib/libCore.so

Dynamic section at offset 0x489d88 contains 37 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libpcre2-8.so.0]
 0x0000000000000001 (NEEDED)             Shared library: [libz.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [liblzma.so.5]
 0x0000000000000001 (NEEDED)             Shared library: [libxxhash.so.0]
 0x0000000000000001 (NEEDED)             Shared library: [liblz4.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libzstd.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libstdc++.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libm.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libgcc_s.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [ld-linux-x86-64.so.2]
 0x000000000000000e (SONAME)             Library soname: [libCore.so]
 0x000000000000001d (RUNPATH)            Library runpath: [$ORIGIN:$ORIGIN/../lib]
```
