#include <iostream>
#include "parser.h"
#include "ppm.h"
#include <math.h>
#include <cfloat>
#include <thread>


using namespace std;
using namespace parser;
typedef unsigned char RGB[3];

double dot(const Vec3f &a, const Vec3f &b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

struct Color
{
    double r, g, b;
    Color() { r = g = b = 0; }
    Color(double i, double j, double k)
    {
        r = i;
        g = j, b = k;
    }
    Color operator+(Color col2){
        Color newCol;
        newCol.r=r+col2.r;
        newCol.g=g+col2.g;
        newCol.b=b+col2.b;

        return newCol;
    }
    Color operator*(Vec3f katsayi){
        Color newCol;
        newCol.r=r*katsayi.x;
        newCol.g=g*katsayi.y;
        newCol.b=b*katsayi.z;
        return newCol;
    }
};

double intersectSphere(const Ray &ray, Sphere &sphere, vector<Vec3f> &my_vertices)
{
    Vec3f centerOfSphere = my_vertices[sphere.center_vertex_id -1];
    float radiusOfSphere = sphere.radius;

    const Vec3f rayOrigin = ray.o;
    const Vec3f rayDirection = ray.d;
    Vec3f originToCenter;

    originToCenter.x = rayOrigin.x - centerOfSphere.x;
    originToCenter.y = rayOrigin.y - centerOfSphere.y;
    originToCenter.z = rayOrigin.z - centerOfSphere.z;

    const double b = 2*dot(rayDirection, originToCenter);
    const double a = dot(rayDirection, rayDirection);
    const double c = dot(originToCenter, originToCenter) - radiusOfSphere * radiusOfSphere;

    double delta = b * b - 4 * a * c;
    double t;
    double t0, t1;

    if (delta < -1){
        return false;
    }
    else if(delta == 0){
        t = -b /(2*a);
    }
    else{
        delta = sqrt(delta);
        t0 = (-b - delta)/(2*a);
        t1 = (-b + delta)/(2*a);
        t = (t0 < t1) ? t0 : t1;
    }
    return t;
}

Ray getRay(Camera camera, int row, int col){

    Vec3f v = camera.up;
    Vec3f gaze = camera.gaze;
    Vec3f u = gaze.cross(v);

    float l = camera.near_plane.x;
    float r = camera.near_plane.y;
    float b = camera.near_plane.z;
    float t = camera.near_plane.w;

    Vec3f m = camera.position + camera.gaze * camera.near_distance;
    Vec3f q = m + u*l + v*t;

    double s_u = (row + 0.5) * (r-l) / camera.image_width;
    double s_v = (col + 0.5) * (t-b)/camera.image_height;

    Vec3f s = q + u*s_u - v*s_v;
    float RayLength=(s-camera.position).length();
    Ray myRay(camera.position, (s - camera.position).normalization());
    myRay.boyu=RayLength;

    return myRay;
}

double intersectTriangle(Ray &r, Triangle &triangle, vector<Vec3f> &my_vertices,int rayOrder) {


    double  a,b,c,d,e,f,g,h,i,j,k,l;
    double beta,gamma,t;

    double eimhf,gfmdi,dhmeg,akmjb,jcmal,blmkc;

    double M;

    double dd;
    Vec3f ma,mb,mc;

    ma = my_vertices[triangle.indices.v0_id-1];
    mb = my_vertices[triangle.indices.v1_id-1];
    mc = my_vertices[triangle.indices.v2_id-1];

    a = ma.x-mb.x;
    b = ma.y-mb.y;
    c = ma.z-mb.z;

    d = ma.x-mc.x;
    e = ma.y-mc.y;
    f = ma.z-mc.z;

    g = r.d.x;
    h = r.d.y;
    i = r.d.z;

    j = ma.x-r.o.x;
    k = ma.y-r.o.y;
    l = ma.z-r.o.z;

    eimhf = e*i-h*f;
    gfmdi = g*f-d*i;
    dhmeg = d*h-e*g;
    akmjb = a*k-j*b;
    jcmal = j*c-a*l;
    blmkc = b*l-k*c;

    M = a*eimhf+b*gfmdi+c*dhmeg;
    if (M==0) return -1;

    t = -(f*akmjb+e*jcmal+d*blmkc)/M;

    if ((rayOrder==1 && t<1) || (rayOrder!=1 && t<0) ) return -1;

    gamma = (i*akmjb+h*jcmal+g*blmkc)/M;

    if (gamma<0 || gamma>1) return -1;

    beta = (j*eimhf+k*gfmdi+l*dhmeg)/M;

    if (beta<0 || beta>(1-gamma)) return -1;

    return t;
}
Color TraceRay(Ray &myRay,int depth,parser::Scene &scene,Vec3f camPos,int  rayOrder) {
    if(depth>scene.max_recursion_depth){
        Color color;

        return color;
    }
    Ray s;
    Vec3f reflectedRayDirection;

    vector<Sphere>& my_spheres =  scene.spheres;
    vector<Vec3f> &my_vertices = scene.vertex_data;
    vector<Triangle>& my_triangles = scene.triangles;
    vector<Mesh> &my_meshes = scene.meshes;
    vector<Material> &my_materials = scene.materials;
    Vec3i &backColor = scene.background_color;
    vector<PointLight>& my_pointlights = scene.point_lights;
    Vec3f &ambientLight = scene.ambient_light;
    double tmin = DBL_MAX;
    int closestObjIndex = -1;
    Triangle myTriangle;
    Material myMaterial;
    Color myColor;
    int materialid;
    Face kesisenFace;
    Vec3f ambientReflectance;
    Vec3f diffuseReflectance;
    Vec3f specularReflectance;
    float phongExp;
    Vec3f normal;
    Vec3f normalLightVector;
    Vec3f notNormalLightVector;
    Vec3f notNormalViewVector;
    double notNormalLightVectorLength;
    Vec3f ViewVector;
    double ViewVectorLength;
    double katsayi;
    Vec3f halfVectorForSpecular;
    Vec3f normalCross;
    Vec3f intensityForDistance;
    Vec3f intersectionPoint;
    Vec3f mirrorReflectance;
    int whichObj=0;
    Triangle kesisenTriangle;
    int r,g,b;
    bool inShadow = false;
    double objectT;
    double shadowRayEpsilon = scene.shadow_ray_epsilon;

    double t=0;
    for(int s=0; s< my_spheres.size() ; s++){

        t= intersectSphere(myRay, my_spheres[s], my_vertices);
        if (((rayOrder==1 && t>myRay.boyu) || (rayOrder!=1 && t>shadowRayEpsilon) ) && t< tmin){

            materialid=my_spheres[s].material_id;
            myMaterial=my_materials[materialid-1];
            ambientReflectance=myMaterial.ambient;
            diffuseReflectance=myMaterial.diffuse;
            specularReflectance=myMaterial.specular;
            phongExp = myMaterial.phong_exponent;
            mirrorReflectance=myMaterial.mirror;
            tmin = t;
            whichObj=1;
            closestObjIndex = s;
        }
    }

    for(int s=0; s< my_triangles.size() ; s++){

        t = intersectTriangle(myRay, my_triangles[s], my_vertices,rayOrder);
        if (((rayOrder==1 && t>myRay.boyu) || (rayOrder!=1 && t>shadowRayEpsilon)) && t< tmin){
            materialid=my_triangles[s].material_id;
            myMaterial=my_materials[materialid-1];
            ambientReflectance=myMaterial.ambient;
            diffuseReflectance=myMaterial.diffuse;
            specularReflectance=myMaterial.specular;
            phongExp = myMaterial.phong_exponent;
            mirrorReflectance=myMaterial.mirror;

            tmin = t;
            whichObj=2;
            closestObjIndex = s;

            kesisenTriangle=my_triangles[s];
        }
    }

    for(int s=0; s< my_meshes.size() ; s++){

        for(Face face : my_meshes[s].faces){

            myTriangle.indices = face;

            t = intersectTriangle(myRay, myTriangle, my_vertices,rayOrder);
            if (((rayOrder==1 && t>myRay.boyu) || (rayOrder!=1 && t>shadowRayEpsilon))&& t< tmin){
                materialid=my_meshes[s].material_id;
                myMaterial=my_materials[materialid-1];
                ambientReflectance=myMaterial.ambient;
                diffuseReflectance=myMaterial.diffuse;
                specularReflectance=myMaterial.specular;
                phongExp = myMaterial.phong_exponent;
                mirrorReflectance=myMaterial.mirror;

                tmin = t;
                whichObj=3;
                closestObjIndex = s;
                kesisenFace=face;
                kesisenTriangle=myTriangle;
            }
        }
    }

    if(closestObjIndex != -1) {

        myColor.r = ambientReflectance.x * ambientLight.x;
        myColor.g = ambientReflectance.y * ambientLight.y;
        myColor.b = ambientReflectance.z * ambientLight.z;

        if (whichObj == 1) {
            intersectionPoint = myRay.d * tmin + myRay.o;
            normal = (intersectionPoint - my_vertices[my_spheres[closestObjIndex].center_vertex_id - 1]) /
                     my_spheres[closestObjIndex].radius;

        }
        else if (whichObj == 2) {
            intersectionPoint = myRay.d * tmin + myRay.o;
            normal = (my_vertices[kesisenTriangle.indices.v2_id - 1] - my_vertices[kesisenTriangle.indices.v1_id - 1]).cross(my_vertices[kesisenTriangle.indices.v0_id - 1] -
                                                                                                                             my_vertices[kesisenTriangle.indices.v1_id - 1]);
            normal = normal.normalization();
        }
        else if (whichObj == 3){
            intersectionPoint = myRay.d * tmin + myRay.o;
            //   normal = (my_vertices[kesisenTriangle.indices.v2_id - 1] - my_vertices[kesisenTriangle.indices.v1_id - 1]).cross(my_vertices[kesisenTriangle.indices.v0_id - 1] -
            //                                                                                                                   my_vertices[kesisenTriangle.indices.v1_id - 1]);
            //  normal = normal.normalization();
            normal= kesisenFace.normal;
        }
        for (int p = 0; p < my_pointlights.size(); p++) {

            notNormalLightVector = (my_pointlights[p].position - intersectionPoint);
            notNormalLightVectorLength = notNormalLightVector.length();
            normalLightVector = notNormalLightVector.normalization();

            inShadow = false;
            objectT=-1;
            // s.o=intersectionPoint + normal*shadowRayEpsilon;
            // s.d=notNormalLightVector;
            //         Ray s(intersectionPoint, notNormalLightVector);
            s.o=intersectionPoint;
            s.d=normalLightVector;
            for(Sphere sphere : my_spheres){
                objectT = intersectSphere(s, sphere, my_vertices);
                if(objectT>shadowRayEpsilon && objectT < notNormalLightVectorLength){
                    inShadow = true;
                    break;
                };
            }
            if(inShadow == false){
                for(Triangle triangle : my_triangles){
                    objectT = intersectTriangle(s, triangle, my_vertices,2);
                    if(objectT>shadowRayEpsilon && objectT < notNormalLightVectorLength){
                        inShadow = true;
                        break;
                    };
                }
            }
            if(inShadow == false){
                for(int m=0; m< my_meshes.size() ; m++){
                    for(Face face : my_meshes[m].faces){
                        myTriangle.indices=face;
                        objectT = intersectTriangle(s, myTriangle, my_vertices,2);
                        if(objectT>shadowRayEpsilon && objectT <notNormalLightVectorLength){
                            inShadow = true;
                            break;
                        };
                    }
                }
            }
//            if(inShadow){
//                continue;
//            }

            katsayi = normal.dot(normalLightVector);
            intensityForDistance = my_pointlights[p].intensity / (notNormalLightVectorLength * notNormalLightVectorLength);
            if (katsayi > 0 && inShadow==false) {
                myColor.r += (float) (diffuseReflectance.x * intensityForDistance.x * katsayi);
                myColor.g += (float) diffuseReflectance.y * intensityForDistance.y * katsayi;
                myColor.b += (float) diffuseReflectance.z * intensityForDistance.z * katsayi;
            }
            ViewVector = camPos- intersectionPoint;
            notNormalViewVector=ViewVector;
            ViewVector = ViewVector.normalization();

            halfVectorForSpecular = (ViewVector + normalLightVector).normalization();
            katsayi = normal.dot(halfVectorForSpecular);
            if(katsayi>0 && inShadow==false){
                myColor.r += (float) (specularReflectance.x * intensityForDistance.x * pow(katsayi, phongExp));
                myColor.g += (float) specularReflectance.y * intensityForDistance.y * pow(katsayi, phongExp);
                myColor.b += (float) specularReflectance.z * intensityForDistance.z * pow(katsayi, phongExp);
            }

            // std::cout<< "recursionda" << std::endl;
            if( mirrorReflectance.x>0 || mirrorReflectance.y>0 || mirrorReflectance.z>0) {
                reflectedRayDirection = (normal * normal.dot(ViewVector) * 2 -ViewVector);
                Ray newRay(intersectionPoint, reflectedRayDirection);
                myColor = myColor + TraceRay(newRay, ++depth, scene, intersectionPoint, 2) * mirrorReflectance;
            }

        }
        myColor.r=(int )(myColor.r < 255 ? myColor.r : 255);
        myColor.g=(int)(myColor.g < 255 ? myColor.g : 255);
        myColor.b=(int)(myColor.b < 255 ? myColor.b : 255);

    }

//else{
//    myColor.r= backColor.x;
//    myColor.g= backColor.y;
//    myColor.b = backColor.z;
//}


    return  myColor;
}

void makeThread( unsigned char *image,parser::Camera camera,parser::Scene scene, int x1,int numOfCores) {
    int width = camera.image_width, height = camera.image_height;
    Ray myRay;
    Color myColor;
    int a=0;

    for (int y = (x1)*height/numOfCores; y < (x1+1)*height/numOfCores; ++y) {

        for (int x = 0; x < width; ++x) {

            myRay = getRay(camera, x, y);

            myColor =TraceRay(myRay, 0, scene, camera.position, 1);
            a=3*(width*y+x);
            image[a++] = (int)myColor.r;
            image[a++] = (int)myColor.g;
            image[a++] = (int)myColor.b;

        }

    }

}

int main(int argc, char* argv[])
{

    parser::Scene scene;
    scene.loadFromXml(argv[1]);
    int progress=0;
    Vec3i backColor=scene.background_color;
    vector<Vec3f> &my_vertices=scene.vertex_data;
    vector<Camera> &my_cameras=scene.cameras;

    for(int i=0;i<scene.meshes.size();i++){
        for(int j=0;j<scene.meshes[i].faces.size();j++) {
            scene.meshes[i].faces[j].normal=((my_vertices[scene.meshes[i].faces[j].v2_id - 1] - my_vertices[scene.meshes[i].faces[j].v1_id - 1])
                    .cross(my_vertices[scene.meshes[i].faces[j].v0_id - 1] -
                           my_vertices[scene.meshes[i].faces[j].v1_id - 1])).normalization();
        }
    }

    for(parser::Camera camera : my_cameras ) {

        int width = camera.image_width, height = camera.image_height;
        unsigned char *image = new unsigned char[width * height * 3];

        int i=0,a=0;

        int numOfCores = std::thread::hardware_concurrency();

        if (numOfCores == 0){
            makeThread(image, camera, scene, 0, 1);
        }
        else{
            std::thread *threads = new std::thread[numOfCores];
            for (int x = 0; x < numOfCores; x++){
                threads[x] = std::thread(&makeThread, std::ref(image), std::ref(camera), std::ref(scene),x,numOfCores);
            }
            for (int x = 0; x < numOfCores; x++){
                threads[x].join();
            }
        }

        write_ppm(camera.image_name.c_str(), image, width, height);
    }
}