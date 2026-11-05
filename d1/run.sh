nasm hello.nas -o helloos.img
qemu-system-x86_64 -drive file=helloos.img,format=raw
