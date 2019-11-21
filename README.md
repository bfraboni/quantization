# Fast gaussian quantization

Fast gaussian blur + Image quantization = Fast gaussian quantization.

This works followed closely the work I have done on [image voronoization](https://github.com/bfraboni/voronoi). The [Codegolf thread](https://codegolf.stackexchange.com/questions/50299/draw-an-image-as-a-voronoi-map) that initiated the aforementionned project links another related [Codegolf thread](https://codegolf.stackexchange.com/questions/42217/paint-by-numbers) "Paint by Numbers", which I again wanted to try. 

The answer provided by DavidC is a Mathematica code that perform a Gaussian blur step then an image quantization. Pretty simple and giving nice results ! I decided to port the exact same routine to C++, using my implementation of the fast gaussian blur approximation by IvanK (see details [here](http://blog.ivank.net/fastest-gaussian-blur.html) and my code [here](https://gist.github.com/bfraboni/946d9456b15cac3170514307cf032a27)) and a classic K-Means pass for image quantization.

Here is the result:

||2|5|10|25|50|100|200|
|:-:|:-:|:-:|:-:|:-:|:-:|:-:|:-:|
|0|![](data/demo/radius0colors2.png)|![](data/demo/radius0colors5.png)|![](data/demo/radius0colors10.png)|![](data/demo/radius0colors25.png)|![](data/demo/radius0colors50.png)|![](data/demo/radius0colors100.png)|![](data/demo/radius0colors200.png)|
|1|![](data/demo/radius1colors2.png)|![](data/demo/radius1colors5.png)|![](data/demo/radius1colors10.png)|![](data/demo/radius1colors25.png)|![](data/demo/radius1colors50.png)|![](data/demo/radius1colors100.png)|![](data/demo/radius1colors200.png)|
|2|![](data/demo/radius2colors2.png)|![](data/demo/radius2colors5.png)|![](data/demo/radius2colors10.png)|![](data/demo/radius2colors25.png)|![](data/demo/radius2colors50.png)|![](data/demo/radius2colors100.png)|![](data/demo/radius2colors200.png)|
|4|![](data/demo/radius4colors2.png)|![](data/demo/radius4colors5.png)|![](data/demo/radius4colors10.png)|![](data/demo/radius4colors25.png)|![](data/demo/radius4colors50.png)|![](data/demo/radius4colors100.png)|![](data/demo/radius4colors200.png)|
|8|![](data/demo/radius8colors2.png)|![](data/demo/radius8colors5.png)|![](data/demo/radius8colors10.png)|![](data/demo/radius8colors25.png)|![](data/demo/radius8colors50.png)|![](data/demo/radius8colors100.png)|![](data/demo/radius8colors200.png)|
|16|![](data/demo/radius16colors2.png)|![](data/demo/radius16colors5.png)|![](data/demo/radius16colors10.png)|![](data/demo/radius16colors25.png)|![](data/demo/radius16colors50.png)|![](data/demo/radius16colors100.png)|![](data/demo/radius16colors200.png)|
|32|![](data/demo/radius32colors2.png)|![](data/demo/radius32colors5.png)|![](data/demo/radius32colors10.png)|![](data/demo/radius32colors25.png)|![](data/demo/radius32colors50.png)|![](data/demo/radius32colors100.png)|![](data/demo/radius32colors200.png)|

