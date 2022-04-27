## Effective System Call Aggregation (ESCA)
The main objective of this work was to reduce the per-syscall overhead through the use of effective syscall aggregation. For that purpose, ESCA takes advantages of system call batching and exploits the parallelism of event-driven applications by leveraging Linux I/O model to overcome the disadvantages of previous solutions.

ESCA is capable of reducing the per-syscall overhead by up to 62% for embedded web servers. Real-world highly concurrent event-driven applications such as Nginx and Redis are known to benefit from ESCA, along with full compatibility with Linux syscall semantics and functionalities.

## Demo
<img src="asset/demo.gif" alt="zigzag" />

Nginx-ESCA shows 11% improvements over vanilla Nginx.

<img src="asset/light-demo.gif" alt="zigzag" />

lighttpd-ESCA shows 13% improvements over vanilla lighttpd.

## Presentation
We also present our work with topic: *Reduce System Call Overhead for Event-Driven Servers* at [Open Source Submit Japan 2021](https://events.linuxfoundation.org/archive/2021/open-source-summit-japan/), organized by the [Linux Foundation](https://www.linuxfoundation.org/).
* [Presentation information](https://ossalsjp21.sched.com/event/peeF)
* [slides](https://static.sched.com/hosted_files/ossalsjp21/c6/Reduce%20System%20Call%20Overhead%20For%20Event%20Driven%20Servers.pdf)
* [video recording](https://youtu.be/_E69oqLsm-0)

## Academic Paper
In [PDP 2022](https://pdp2022.infor.uva.es/)
*[Yu-Cheng Cheng](https://github.com/eecheng87), [Ching-Chun (Jim) Huang](https://github.com/jserv), Chia-Heng Tu*
National Cheng Kung University, Taiwan.

The paper is available in the [IEEE Xplore](https://ieeexplore.ieee.org/abstract/document/9756707) digital library, and you can get a <a href="main.pdf" target="_blank">preprint copy</a>.

## Citation

If you find this work useful in your research, please cite:
```
@inproceedings{cheng2022esca,
    author={Cheng, Yu-Cheng and Huang, Ching-Chun (Jim) and Tu, Chia-Heng},
    booktitle={2022 30th Euromicro International Conference on Parallel, Distributed and Network-based Processing (PDP)},
    title={ESCA: Effective System Call Aggregation for Event-Driven Servers},
    year={2022},
    pages={18-25},
    doi={10.1109/PDP55904.2022.00012}
}
```
