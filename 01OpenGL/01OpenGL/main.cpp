

#include "GLShaderManager.h"
#include "GLTools.h"
#include <GLUT/GLUT.h>


//定义一个，着色管理器
GLShaderManager shaderManager;
//简单的批次容器，是GLTools的一个简单的容器类。
GLBatch triangleBatch;

//blockSize 边长
GLfloat blockSize = 0.2f;
//正方形的4个点坐标
GLfloat vVerts[] = {
        -blockSize,-blockSize,0.0f,
        blockSize,-blockSize,0.0f,
        blockSize,blockSize,0.0f,
        -blockSize,blockSize,0.0f
};



/*
 在窗口大小改变时，接收新的宽度&高度。
 */
void changeSize(int w,int h) {
    glViewport(0, 0, w, h);
}

void RenderScene(void) {
    //1.清除一个或者一组特定的缓存区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    //2.设置一组浮点数来表示红色
    GLfloat vRed[] = {1.0,0.0,0.0,1.0f};
    
    //传递到存储着色器，即GLT_SHADER_IDENTITY着色器，这个着色器只是使用指定颜色以默认笛卡尔坐标第在屏幕上渲染几何图形
    shaderManager.UseStockShader(GLT_SHADER_IDENTITY,vRed);
    
    //提交着色器
    triangleBatch.Draw();

    //将后台缓冲区进行渲染，然后结束后交换给前台
    glutSwapBuffers();
}

void setupRC() {
    //设置清屏颜色（背景颜色）
    glClearColor(0.0f, 0.0f, 0.7f, 1);
    
    //没有着色器，在OpenGL 核心框架中是无法进行任何渲染的。初始化一个渲染管理器。
    //在前面的课程，我们会采用固管线渲染，后面会学着用OpenGL着色语言来写着色器
    shaderManager.InitializeStockShaders();
    
    //修改为GL_TRIANGLE_FAN ，4个顶点
    triangleBatch.Begin(GL_TRIANGLE_FAN, 4);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();
}

// 特殊函数回调
void SpecialKeys(int key, int x, int y){
    // 设置每一步的距离，就是键盘按一次，图形移动的距离
    GLfloat stepSize = 0.05f;
    
    // 取任意一点的x和y坐标，当作初始坐标
    // 这里取正方形左上角定点的坐标
    GLfloat startX = vVerts[9];
    GLfloat startY = vVerts[10];
    
    // 按键：上
    if (key == GLUT_KEY_UP) {
        startY += stepSize;
    }
    // 按键：下
    if (key == GLUT_KEY_DOWN) {
        startY -= stepSize;
    }
    // 按键：左
    if (key == GLUT_KEY_LEFT) {
        startX -= stepSize;
    }
    // 按键：右
    if (key == GLUT_KEY_RIGHT) {
        startX += stepSize;
    }
      
    // 边缘碰撞处理
    //当正方形移动超过最左边的时候
    if (startX < -1.0f) {
        startX = -1.0f;
    }
    //当正方形移动到最右边时
    //1.0 - blockSize * 2 = 总边长 - 正方形的边长 = 最左边点的位置
    if (startX > (1.0 - blockSize * 2)) {
        startX = 1.0f - blockSize * 2;
    }
    //当正方形移动到最下面时
    //-1.0 - blockSize * 2 = Y（负轴边界） - 正方形边长 = 最下面点的位置
    if (startY < -1.0f + blockSize * 2 ) {
        startY = -1.0f + blockSize * 2;
    }
    //当正方形移动到最上面时
    if (startY > 1.0f) {
        startY = 1.0f;
    }
    
    
    // 更新四个顶点坐标， 普通更新方法，顶点数量少时，可以使用
    // 左上角
    vVerts[9] = startX;
    vVerts[10] = startY;
    // 左下角
    vVerts[0] = vVerts[9];
    vVerts[1] = vVerts[10] - blockSize*2;
    // 右下角
    vVerts[3] = vVerts[0] + blockSize*2;
    vVerts[4] = vVerts[1];
    // 右上角
    vVerts[6] = vVerts[3];
    vVerts[7] = vVerts[10];
    
    printf("startX = %f, startY = %f\n", startX, startY);
    
    triangleBatch.CopyVertexData3f(vVerts);
    // 标记当前需要重新绘制
    glutPostRedisplay();
}

int main(int argc,char *argv[]) {
    //设置当前工作目录
    gltSetWorkingDirectory(argv[0]);
    
    //初始化GLUT库,这个函数只是传说命令参数并且初始化glut库
    glutInit(&argc, argv);
    
    /*
     初始化双缓冲窗口，其中标志GLUT_DOUBLE、GLUT_RGBA、GLUT_DEPTH、GLUT_STENCIL分别指
     双缓冲窗口、RGBA颜色模式、深度测试、模板缓冲区
     
     --GLUT_DOUBLE`：双缓存窗口，是指绘图命令实际上是离屏缓存区执行的，然后迅速转换成窗口视图，这种方式，经常用来生成动画效果；
     --GLUT_DEPTH`：标志将一个深度缓存区分配为显示的一部分，因此我们能够执行深度测试；
     --GLUT_STENCIL`：确保我们也会有一个可用的模板缓存区。
     深度、模板测试后面会细致讲到
     */
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_STENCIL);
    
    //GLUT窗口大小、窗口标题
    glutInitWindowSize(500, 500);
    glutCreateWindow("Triangle");
    
    /*
     GLUT 内部运行一个本地消息循环，拦截适当的消息。然后调用我们不同时间注册的回调函数。我们一共注册2个回调函数：
     1）为窗口改变大小而设置的一个回调函数
     2）包含OpenGL 渲染的回调函数
     */
    //注册重塑函数
    glutReshapeFunc(changeSize);
    //注册显示函数
    glutDisplayFunc(RenderScene);
    //注册特殊函数
    glutSpecialFunc(SpecialKeys);
    
    /*
     初始化一个GLEW库,确保OpenGL API对程序完全可用。
     在试图做任何渲染之前，要检查确定驱动程序的初始化过程中没有任何问题
     */
    GLenum status = glewInit();
    if (GLEW_OK != status) {
        printf("GLEW Error:%s\n",glewGetErrorString(status));
        return 1;
    }
    
    //设置我们的渲染环境
    setupRC();
    
    glutMainLoop();
    return  0;
}
