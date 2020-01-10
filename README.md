# Fast gaussian quantization

Fast gaussian blur + Image quantization = Fast gaussian quantization.

This works followed closely the work I have done on [image voronoization](https://github.com/bfraboni/voronoi). The [Codegolf thread](https://codegolf.stackexchange.com/questions/50299/draw-an-image-as-a-voronoi-map) that initiated the aforementionned project links another related [Codegolf thread](https://codegolf.stackexchange.com/questions/42217/paint-by-numbers) "Paint by Numbers", which I again wanted to try. 

The answer provided by DavidC is a Mathematica code that perform a Gaussian blur step then an image quantization. Pretty simple and giving nice results ! I decided to port the exact same routine to C++, using my implementation of the fast gaussian blur approximation by IvanK (see details [here](http://blog.ivank.net/fastest-gaussian-blur.html) and my code [here](https://gist.github.com/bfraboni/946d9456b15cac3170514307cf032a27)) and a classic K-Means pass for image quantization.

Here is the result:


|R\K|4|16|64|256|
|:-:|:-:|:-:|:-:|:-:|
|0|![](data/demo2/radius0colors4.png)|![](data/demo2/radius0colors16.png)|![](data/demo2/radius0colors64.png)|![](data/demo2/radius0colors256.png)|
|1|![](data/demo2/radius1colors4.png)|![](data/demo2/radius1colors16.png)|![](data/demo2/radius1colors64.png)|![](data/demo2/radius1colors256.png)|
|2|![](data/demo2/radius2colors4.png)|![](data/demo2/radius2colors16.png)|![](data/demo2/radius2colors64.png)|![](data/demo2/radius2colors256.png)|
|4|![](data/demo2/radius4colors4.png)|![](data/demo2/radius4colors16.png)|![](data/demo2/radius4colors64.png)|![](data/demo2/radius4colors256.png)|
|8|![](data/demo2/radius8colors4.png)|![](data/demo2/radius8colors16.png)|![](data/demo2/radius8colors64.png)|![](data/demo2/radius8colors256.png)|
|16|![](data/demo2/radius16colors4.png)|![](data/demo2/radius16colors16.png)|![](data/demo2/radius16colors64.png)|![](data/demo2/radius16colors256.png)|
