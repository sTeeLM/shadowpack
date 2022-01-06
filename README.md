# A tool for general steganography #
##  What it can do: ##
- Photo Image Files: BMP/PNG/TIFF/PPM/JPEG
- Audio Files: (still working)
- Vidio Files: (still working)
- Big file support: use disk cache (4G limit on win32)
- Encryption: AES/SEED/CAMELLIA/BLOWFISH/CAST/IDEA/RC4/3DES

# 通用隐写工具 #
## 功能 ##
- 向各种图片文件隐写数据: BMP/PNG/TIFF/PPM/JPEG，
-   只支持RGB或者RGBA、8bits/channel的图像，JPEG不支持JPEG2K，JBIG，TIFF不支持有损压缩（其实可以没有这些限制的，实在不想写了。。）
- 向各种音频文件隐写数据: (still working)
-   各种位宽的PCM或者PCM无损压缩格式（FLAC、ALAC等等）查分编码等有损压缩不支持（其实也可以没有这些限制的，实在不想写了。。），MP3还在研究
- 向各种视频文件隐写数据: (still working)
-   不敢讲有一搭没一搭的啥时候做。。。我会，就是懒得搞
- 大文件支持: 使用硬盘cache (32位windows上不能大于4G)
- 各种加密算法支持: AES/SEED/CAMELLIA/BLOWFISH/CAST/IDEA/RC4/3DES

## 扩展性 ##
- 代码结构还算凑合吧，有兴趣可以自己fork做做扩展，求提PR

## 大致原理 ##
- 无他（唯手熟尔，可能用到一点大学信号处理、信息论知识？），就是各种规则写时域、空域数据的LSB
- 对于JPEG、MP3格式，按一定规则修改频域DCT参数的LSB
- 多种格式选择，一般来讲越隐秘，可写空间越小，可以自由权衡

## 隐写数据能被检测出来么？ ##
- 事实上是给数据加了一些噪声，人眼和人耳还是不好分辨出来的
- 程序是否可检测：如果没有原始数据对比，可能还是挺难的，特别是数据加密之后就是白噪声，理论上通过信号处理可以发现白噪声，但是不好讲这些噪声是原来就有的，还是故意加的数据。。。
- 即使用相同的隐写工具，改换了加密口令，数据也是完全不同的，并且没有任何特征可循（本程序自己要知道有没有数据可读，也是各种尝试加密+隐写格式的组合）

