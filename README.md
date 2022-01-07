# A tool for general steganography #
##  What it can do: ##
- Photo Image Files: BMP/PNG/TIFF/PPM/JPEG
- Audio Files: (still working)
- Vidio Files: (still working)
- Big file support: use disk cache (4G limit on win32)
- Encryption: AES/SEED/CAMELLIA/BLOWFISH/CAST/IDEA/RC4/3DES

# 通用隐写工具 #
## 功能 ##
- 向各种常见图片文件隐写数据: BMP/PNG/TIFF/PPM/JPEG，
  1. 只支持RGB或者RGBA、8bits/channel的真彩色图片
  2. （其实上述也可以支持的，考虑到非真彩色的图片很少了，不想写）
  2. 不支持JPEG2K，JBIG
  3. TIFF各种压缩方法不支持有损压缩
- 向各种音频文件隐写数据: 
  1. 各种位宽的PCM或者PCM无损压缩格式（FLAC、ALAC等等）
  2. 差分编码等有损压缩不支持
  3. （其实也可以没有这些限制的，实在不想写了。。）
  4. MP3还在研究
- 向各种视频文件隐写数据: (still working)
  1. 不敢讲有一搭没一搭的啥时候做。。。我应该能搞，就是懒得搞
- 支持大文件: 
  1. 目标文件太大可以选择使用硬盘cache
  2. win32硬盘cache不能大于4G，win64无限制（因为偷懒直接使用了mmap）
  3. 数据文件最大支持4G
- 各种加密算法支持: 
  1. AES/SEED/CAMELLIA/BLOWFISH/CAST/IDEA/RC4/3DES
  2. 实现上使用ctr128，8位block的分组加密做了特别的处理
- 目前不打算干的事情：
  1. 压缩：有各种很好的压缩软件，请自行压缩后再隐写
  2. 在目标文件上做文件系统支持目录结构：纯属炫技，隐写前不能自己打个包么？

## 扩展性 ##
- 代码结构还算凑合吧，有兴趣可以自己fork做做扩展，求提PR

## 大致原理 ##
- 无他（唯手熟尔，可能用到一点大学信号处理、信息论知识？），就是各种规则写时域、空域数据的LSB
- 对于JPEG、MP3格式，按一定规则修改频域DCT参数的LSB
- 多种格式选择，一般来讲越隐秘，可写空间越小，可以自由权衡

## 隐写数据能被检测出来么？ ##
- 人是否能检测：
  1. 本质是给原有数据加了一些噪声，如果不是数据塞太多（用了更激进的格式）人眼和人耳还是不好分辨出来的
- 程序是否可检测：
  1. 如果没有原始文件对比，可能还是挺难的，特别是数据加密之后就是白噪声
  2. 理论上通过信号处理可以发现白噪声，但是不好讲这些噪声是原来就有的，还是故意加的数据。。。
  3. 即使用相同的隐写工具，改换了加密口令，数据也是完全不同的，并且没有任何特征（例如MAGIC Number）可循

