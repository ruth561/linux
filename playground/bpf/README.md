# eBPF Program for BPF_PROG_TYPE_RUTH

カーネルに新しいBPFプログラムタイプ「BPF_PROG_TYPE_RUTH」を追加実装したので、そこにアタッチするためのBPFプログラムを実装した。カーネルのロードやアタッチにlibbpfは使っておらず、システムコールから実装している。

# ビルド方法

ビルドにはカーネルヘッダーが必要なので、まず最初に以下のコマンドでカーネルヘッダを手元にインストールする。
```
$ make kernel-headers
```
実行バイナリをビルドするには以下のコマンドを実行する。
```
$ make
```
QEMUで動作確認するためには以下のコマンドを実行する。QEMUが起動すると `/misc/ruth` というファイルが存在しており、これを実行することでBPFプログラムをアタッチすることができる。
```
$ make qemu
```


# compile_commands.json

VS Codeで開発しているときは、カーネルソースツリーのルートに置いてある `compile_commands.json`（`scripts/clang-tools/gen_compile_commands.py ` によって自動生成可能）に `ruth.c` のエントリを追加してあげる必要がある。追加するエントリは以下のコマンドで入手できる。
```
$ make gen-entry-for-compile-commands.json
```
