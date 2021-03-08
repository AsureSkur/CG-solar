本程序的依赖库为glfw，glew，glm。

效果为太阳系八大行星绕太阳做环绕运动。

可以双击文件中myfirstopengl.exe查看程序运行效果。
注：exe文件需要和Default.frag，Default.vert，light.frag，light.vert在同一目录下运行，否则顶点着色器和片段着色器无法加载，将导致画面空白。

为了较好的观赏效果，项目中八大行星大小、距离与真实情况存在一定差异。

太阳设定为红色，并发出红色光照。

使用WSAD键可以上下左右移动，鼠标的移动可以控制视角。

本项目的源代码：myfirstopengl.cpp
顶点着色器：Default.vert和light.vert
片段着色器：Default.frag和light.frag
