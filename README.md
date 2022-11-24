# Guest Kernel Optimized for GiantVM
This is the kernel source that can be used for GiantVM environments with optimized performance. It includes the following improvements:

* Realignment of kernel data to avoid false sharing
* Implementation of a per-node RW semaphore for mmap_sem, which reduces DSM overheads caused by the contention for the semaphore variable
  * To use this feature, you should enable MMSEM and configure MMSEM_NODE(default:4) and MMSEM_CPU(default:16) according to your system

Contributors: MemOS project members in Electronics and Telecommunications Research Institute (ETRI), South Korea
* Baik Song An (bsahn@etri.re.kr)
* Myung Hoon Cha (mhcha@etri.re.kr)
* Hong Yeon Kim (kimhy@etri.re.kr)
* Sang-Min Lee (sangmin2@etri.re.kr)

-----------------------------------------
License
* GPL v2.0
