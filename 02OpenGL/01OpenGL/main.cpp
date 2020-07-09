
#include "GLShaderManager.h"
#include "GLTools.h"
#include <GLUT/GLUT.h>

//定义一个，着色管理器
GLShaderManager shaderManager;
//简单的批次容器，是GLTools的一个简单的容器类。
GLBatch triangleBatch;


//blockSize 边长
GLfloat blockSize = 0.1f;
//正方形的4个点坐标
GLfloat vVerts[] = {
        -blockSize,-blockSize,0.0f,
        blockSize,-blockSize,0.0f,
        blockSize,blockSize,0.0f,
        -blockSize,blockSize,0.0f
};

GLfloat xPos = 0.0f;
GLfloat yPos = 0.0f;


/*
 在窗口大小改变时，接收新的宽度&高度。
 */
void changeSize(int w,int h) {
    /*
      x,y 参数代表窗口中视图的左下角坐标，而宽度、高度是像素为表示，通常x,y 都是为0
     */
    glViewport(0, 0, w, h);
}

void RenderScene(void) {

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    
    GLfloat vRed[] = {1.0f,0.0f,0.0f,0.0f};
    
    M3DMatrix44f mFinalTransform,mTransfromMatrix,mRotationMartix;
    
    //平移
    m3dTranslationMatrix44(mTransfromMatrix, xPos, yPos, 0.0f);
    
    //每次平移时，旋转5度
    static float yRot = 0.0f;
    yRot += 5.0f;
    m3dRotationMatrix44(mRotationMartix, m3dDegToRad(yRot), 0.0f, 0.0f, 1.0f);
    
    //将旋转和移动的矩阵结果 合并到mFinalTransform （矩阵相乘）
    m3dMatrixMultiply44(mFinalTransform, mTransfromMatrix, mRotationMartix);
    
    //将矩阵结果 提交给固定着色器（平面着色器）中绘制
    shaderManager.UseStockShader(GLT_SHADER_FLAT,mFinalTransform,vRed);
    triangleBatch.Draw();
    
    //执行交换缓存区
    glutSwapBuffers();
}

void setupRC() {
    //设置清屏颜色（背景颜色）
    glClearColor(0.98f, 0.40f, 0.7f, 1);
    
    //没有着色器，在OpenGL 核心框架中是无法进行任何渲染的。初始化一个渲染管理器。
    shaderManager.InitializeStockShaders();
    
    //修改为GL_TRIANGLE_FAN ，4个顶点
    triangleBatch.Begin(GL_TRIANGLE_FAN, 4);
    triangleBatch.CopyVertexData3f(vVerts);
    triangleBatch.End();
}

void SpecialKeys(int key, int x, int y) {
    GLfloat stepSize = 0.025f;
    
    if (key == GLUT_KEY_UP) {
        yPos += stepSize;
    }
    
    if (key == GLUT_KEY_DOWN) {
        yPos -= stepSize;
    }
    
    if (key == GLUT_KEY_LEFT) {
        xPos -= stepSize;
    }
    
    if (key == GLUT_KEY_RIGHT) {
        xPos += stepSize;
    }
    
    //碰撞检测
    if (xPos < (-1.0f + blockSize)) {
        xPos = -1.0f + blockSize;
    }
    
    if (xPos > (1.0f - blockSize)) {
        xPos = 1.0f - blockSize;
    }
    
    if (yPos < (-1.0f + blockSize)) {
        yPos = -1.0f + blockSize;
    }
    
    if (yPos > (1.0f - blockSize)) {
        yPos = 1.0f - blockSize;
    }
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
    glutInitWindowSize(800, 600);
    glutCreateWindow("Triangle");

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
