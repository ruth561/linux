# このディレクトリについて

Linux Kernelの改造に関する実装やメモをまとめておく「遊び場」。

# 環境構築

## rootfs

QEMUでカーネルを起動させるためにはrootfsが必要になってくる。簡易的なrootfsを作るために、BusyBoxを利用する（[参考サイト](https://cylab.be/blog/320/build-a-kernel-initramfs-and-busybox-to-create-your-own-micro-linux)）。

## initプログラム

上記の方法で環境構築をすると、最初に実行されるプログラムは/initとなる。そこで、/initに記述するべき実装を以下に示す。

最低限必要な実装は、以下の通り。このとき、/sbin/initから/bin/busyboxへのシンボリックリンクを張っておくことを忘れずに。

```bash
#!/bin/sh

mount -t devtmpfs devtmpfs /dev
mount -t proc none /proc
mount -t sysfs none /sys

# dmesgのバッファを空にする。
dmesg -c > /dev/null
# シェルをrootで起動する。
# 単に`/bin/sh`としていない理由は、起動時のエラーメッセージを
# 表示させないため。
setsid cttyhack setuidgid 0 sh
# シェルが終了したらシステムを終了する。
# これがないと、シェルの終了時にカーネルパニックを引き起こす。
poweroff -d 0 -f
```

## makeコマンド

### rootfs.cpio

rootfsのCPIOアーカイブを作成する。このコマンドを実行する前に、rootfs/というサブディレクトリを作成し、その配下にBusyBoxの_install/ディレクトリをコピーしておく（詳しくは上のリンク先の説明を参照）。
rootfsに変更を加えてから`make rootfs.cpio`とすることで、変更をCPIOファイルに適用することができる。

### qemu

QEMUでカーネルを起動する。起動時のオプションなどを変更したい場合は、make変数の`QEMU_ARGS`に変更を加えると良い。
