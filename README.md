# seam

<p align="center">
  <img src="https://github.com/brekekekex/seam/blob/master/gallery/editor.gif" width="400" />
</p>

## Overview
*Seam carving* is a method of content-aware image resizing first presented by Shai Avidan and Ariel Shamir in a [landmark paper](https://dl.acm.org/doi/10.1145/1275808.1276390) in 2007. A *seam* of an image *I* is defined as a monotonic and connected path of pixels walked from the leftmost column of *I* to the rightmost column (or from top to bottom). By imposing an energy function *E(i,j)* upon *I*, one effectively reformulates the image resizing problem as a brute-force search for minimal-energy seams (which is made practical through dynamic programming). The following repository details the implementation of *Seam*, an interactive editor and visualizer for said operation. In particular, *Seam* implements the improved, forward-energy formulation of seam carving published by Michael Rubinstein, Shamir, and Avidan in [2008](https://dl.acm.org/doi/10.1145/1360612.1360615).

## Design & Features
*Seam* is a minimal viable product hastily written in C, self-contained with the exception of: i) an interface built on [SDL2](https://www.libsdl.org/index.php); and ii) image decoding/encoding performed through Lode Vandevenne's lightweight [LodePNG](https://lodev.org/lodepng/). 

Per iteration, *Seam* computes a discrete gradient through convolution with a [Sobel kernel](https://en.wikipedia.org/wiki/Sobel_operator), which is then used to compute an accumulative cost matrix *M* (as in dynamic programming). The optimal solution is then back-traced through *M* to yield a seam of pixels which may be deleted from *I* to produce an image one column (or row) smaller than *I*.      

The result is a resizing operation that *may* be less destructive of source features than linear or cubic interpolation:

<div>
  <p float="left">
    <img src="https://github.com/brekekekex/seam/blob/master/gallery/kanagawa_seam_compressed.gif" width="400" />
    <img src="https://github.com/brekekekex/seam/blob/master/gallery/kanagawa_energy_compressed.gif" width="400" /> 
  </p>
</div>

(Left: Seam carving animation generated with the *-rs* flag. Right: Corresponding heatmap animation generated with the *-re* flag.)

<div>
  <p float="left">
    <img src="https://github.com/brekekekex/seam/blob/master/gallery/kanagawa_linear.png" width="400" />
    <img src="https://github.com/brekekekex/seam/blob/master/gallery/kanagawa_sc.png" width="400" /> 
  </p>
</div>

(Left: Result of linear interpolation. Right: Result of seam carving (note the aberration around Mt. Fuji). Source: Hokusai, [*The Great Wave off Kanagawa*](https://en.wikipedia.org/wiki/The_Great_Wave_off_Kanagawa#/media/File:Tsunami_by_hokusai_19th_century.jpg)) 

<div>
  <p float="left">
    <img src="https://github.com/brekekekex/seam/blob/master/gallery/last_supper_seam_compressed.gif" width="400" />
    <img src="https://github.com/brekekekex/seam/blob/master/gallery/last_supper_energy_compressed.gif" width="400" /> 
  </p>
</div>

<div>
  <p float="left">
    <img src="https://github.com/brekekekex/seam/blob/master/gallery/last_supper_linear.png" width="400" />
    <img src="https://github.com/brekekekex/seam/blob/master/gallery/last_supper_sc.png" width="400" /> 
  </p>
</div>

(Left: Result of linear interpolation. Right: Result of seam carving (note that the head of Jesus is protected by a high energy region). Source: Leonardo da Vinci, [*The Last Supper*](https://en.wikipedia.org/wiki/The_Last_Supper_(Leonardo)#/media/File:The_Last_Supper_-_Leonardo_Da_Vinci_-_High_Resolution_32x16.jpg)) 

## Gallery
<div>
<p float="left">
  <img src="https://github.com/brekekekex/seam/blob/master/gallery/american_gothic_seam_compressed.gif" width="400" />
  <img src="https://github.com/brekekekex/seam/blob/master/gallery/american_gothic_energy_compressed.gif" width="400" /> 
</p>
</div>

(Source: Grant Wood, [*American Gothic*](https://en.wikipedia.org/wiki/American_Gothic#/media/File:Grant_Wood_-_American_Gothic_-_Google_Art_Project.jpg)) 


