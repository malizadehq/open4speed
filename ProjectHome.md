# Overview #
This game brings a new kind of gaming, there are no payments, no autogas. Your score depends only on your skills. You have to control your car as good as you can because the game provides gameplay without any secret helpers.
The game is powered with a such nice graphics with reflections, motion blur and particle effects.
The game engine is based on Bullet Physics 2.82 and OpenGL ES 2.0. For better compatibility you can change visual detail in options.

[screenshot](https://lh5.ggpht.com/A-6eFpW9FF0DkfKyJgCdXekG2IdVQgFUJQMNgaQzR83iOFAJdi8roiLWnqRHx8VujDc=h900-rw)

### Libraries ###
For compiling you need those libs:<br>
freeglut3-dev<br>
libbullet-dev<br>
libzip-dev<br>
libpng-dev<br>
libglm-dev<br>
<br>
<b>Note:</b> If libbullet is not avaible in your distro repository, you can compile it itself from support/bullet...<br>
<br>
<h3>Compiling</h3>
To compile code enter the jni directory and run<br>
qmake<br>
make<br>
<br>
<b>Note:</b> If you are using 64bit OS, you have to replace in the in jni/open4speedHD.pro<br>
<i>../support/fmodapi-4.44.08/libfmodex-4.44.08.so</i><br>
with<br>
<i>../support/fmodapi-4.44.08/libfmodex64-4.44.08.so</i><br>