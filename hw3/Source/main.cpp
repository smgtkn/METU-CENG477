#include <iostream>
#include "parser.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using parser::Vec3f;
using namespace std;
//////-------- Global Variables -------/////////

GLuint gpuVertexBuffer;
GLuint gpuNormalBuffer;
GLuint gpuIndexBuffer;

// Sample usage for reading an XML scene file
parser::Scene scene;
static GLFWwindow* win = NULL;

static void errorCallback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void transformationinfo(const std::vector<parser::Transformation> & trans) {
   glPushMatrix();
    parser::Vec3f translationvec;
    parser::Vec3f scalingvec;
    parser::Vec4f rotationvec;
    int size=trans.size();
    size--;
    for(int i=size;i>-1;i--){
        if(trans[i].transformation_type=="Translation"){
            translationvec=scene.translations[trans[i].id-1];
            glTranslatef(translationvec.x,translationvec.y,translationvec.z);
        }
        else if(trans[i].transformation_type=="Scaling"){
            scalingvec=scene.scalings[trans[i].id-1];
            glScalef(scalingvec.x,scalingvec.y,scalingvec.z);

        }
        else if (trans[i].transformation_type=="Rotation"){
            rotationvec=scene.rotations[trans[i].id-1];
            glRotatef(rotationvec.x,rotationvec.y,rotationvec.z,rotationvec.w);

        }



    }


}

double lastTime;
int nbFrames;

void showFPS(GLFWwindow *pWindow){
    // Measure speed
     double currentTime = glfwGetTime();
     double delta = currentTime - lastTime;
	 char ss[500] = {};
     nbFrames++;
     if ( delta >= 1.0 ){ // If last cout was more than 1 sec ago
         //cout << 1000.0/double(nbFrames) << endl;

         double fps = ((double)(nbFrames)) / delta;

         sprintf(ss,"CENG477 - HW3 [%lf FPS]",fps);

         glfwSetWindowTitle(pWindow, ss);

         nbFrames = 0;
         lastTime = currentTime;
     }
}


void displayScene(){

    static bool firstTime = true;

    static int vertexPosDataSizeInBytes;

    if(firstTime){

        firstTime = false;
        vertexPosDataSizeInBytes = scene.vertex_data.size() * 3 * sizeof(GLfloat);
        int indexDataSizeInBytes = scene.numberOfFaces * 3 * sizeof(GLuint);
        GLuint* indices = scene.indices;
        GLfloat *vertexPos=scene.vertexPos;
        GLfloat  * normals=scene.normals;

        GLuint vertexAttribBuffer, indexBuffer;

        glGenBuffers(1, &vertexAttribBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexAttribBuffer);
        glBufferData(GL_ARRAY_BUFFER, 2*vertexPosDataSizeInBytes, 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertexPosDataSizeInBytes, vertexPos);
        glBufferSubData(GL_ARRAY_BUFFER,vertexPosDataSizeInBytes,vertexPosDataSizeInBytes,normals);
      //  glDrawElements(GL_TRIANGLES, scene.vertex_data.size(), GL_UNSIGNED_INT, 0);
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSizeInBytes, indices, GL_STATIC_DRAW);
        delete [] scene.indices;
        delete [] scene.normals;
        delete [] scene.vertexPos;

    }

    glVertexPointer(3, GL_FLOAT, 0, 0);
    glNormalPointer(GL_FLOAT, 0, (void*)vertexPosDataSizeInBytes);


    int offset=0;
    for(const auto & mesh : scene.meshes){

        const auto material = scene.materials[mesh.material_id];
        GLfloat ambient[4] ;
        ambient[0]=material.ambient.x;
        ambient[1]=material.ambient.y;
        ambient[2]=material.ambient.z;
        ambient[3]=1.0;
        GLfloat diffuse[4] ;
       diffuse[0]=material.diffuse.x;
        diffuse[1]=material.diffuse.y;
        diffuse[2]=material.diffuse.z;
        diffuse[3]=1.0;
        GLfloat spec[4] ;
        spec[0]=material.specular.x;
        spec[1]=material.specular.y;
        spec[2]=material.specular.z;
        spec[3]=1.0;


        GLfloat phong[1];
         phong[0]=material.phong_exponent;

        glMaterialfv ( GL_FRONT , GL_AMBIENT , ambient );
        glMaterialfv ( GL_FRONT , GL_DIFFUSE , diffuse );
        glMaterialfv ( GL_FRONT , GL_SPECULAR , spec);
        glMaterialfv ( GL_FRONT , GL_SHININESS , phong );



        if(mesh.mesh_type == "Wireframe"){
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else if(mesh.mesh_type=="Solid"){
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        transformationinfo(mesh.transformations);
        glDrawElements(GL_TRIANGLES, mesh.faces.size()*3, GL_UNSIGNED_INT, (const void *)(offset*3*sizeof(GLuint)));

        offset = offset + mesh.faces.size();
        glPopMatrix();
        //glDrawElements(GL_TRIANGLES, scene.numberOfFaces*3, GL_UNSIGNED_INT,0);
    }
    

}

void clearAndDisplay(){

    glClearColor(scene.background_color.x, scene.background_color.y,
               scene.background_color.z, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    displayScene();
    // displayOneTriangle();
}

void initializeDisplay(){
    
    glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    if(scene.culling_enabled == 1){

        glEnable(GL_CULL_FACE);

        if(scene.culling_face == 0){
            glCullFace(GL_BACK);
        }
        else{
            glCullFace(GL_FRONT);
        }
    }
    
}

void adjustCamera(){

    const parser::Camera camera = scene.camera;
    const Vec3f eye = camera.position;
    const Vec3f center = eye + camera.gaze * camera.near_distance;
    const Vec3f v = camera.up;
    const parser::Vec4f near_plane = camera.near_plane;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(near_plane.x, near_plane.y, near_plane.z, near_plane.w,
                camera.near_distance, camera.far_distance);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, v.x, v.y, v.z);
}

void adjustLights(){

     GLfloat ambient[4] ;
     ambient[0]=scene.ambient_light.x;
     ambient[1]=scene.ambient_light.y;
     ambient[2]=scene.ambient_light.z;
     ambient[3]=1.0;
                              

    
    for (size_t i = 0; i < scene.point_lights.size(); i++) {
        parser::PointLight light = scene.point_lights[i];
        GLfloat intensity[4];
        intensity[0] = light.intensity.x;
        intensity[1]=light.intensity.y;
        intensity[2]=light.intensity.z;
        intensity[3]=1.0;
        glLightfv(GL_LIGHT0 + i, GL_AMBIENT, ambient);
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, intensity);
        glLightfv(GL_LIGHT0 + i, GL_SPECULAR, intensity);
        glEnable(GL_LIGHT0 + i);
    }
}

int main(int argc, char* argv[]) {
    scene.loadFromXml(argv[1]);

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    win = glfwCreateWindow(scene.camera.image_width, scene.camera.image_height, "CENG477 - HW3", NULL, NULL);
    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(win);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(win, keyCallback);

    initializeDisplay();
    adjustLights();
    adjustCamera();

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while(!glfwWindowShouldClose(win)) {
        

        clearAndDisplay();
        showFPS(win);
        glfwSwapBuffers(win);
        glfwWaitEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
