#ifndef __HW1__PARSER__
#define __HW1__PARSER__

#include <string>
#include <vector>
#include <ostream>

namespace parser
{
    struct Vec3f
    {
        float x, y, z;
        friend std::ostream &operator<<(std::ostream &os, const Vec3f &f);
      //  Vec3f();
        Vec3f operator + (const Vec3f& v) const;
        Vec3f operator - (const Vec3f& v) ;
        Vec3f operator * (double d) const;
        Vec3f operator / (double d) ;
        Vec3f cross (const Vec3f& v) ;
        float dot (const Vec3f& v) ;
        double length();
        Vec3f normalization();

    };

    struct Ray
    {
        Vec3f o, d;
        Ray(const Vec3f &ori, const Vec3f &dir) : o(ori), d(dir) {}
        float boyu;

        Ray() {}
    };

    struct Vec3i
    {
        int x, y, z;
    };

    struct Vec4f
    {
        float x, y, z, w;
    };

    struct Camera
    {
        Vec3f position;
        Vec3f gaze;
        Vec3f up;
        Vec4f near_plane;
        float near_distance;
        int image_width, image_height;
        std::string image_name;
    };

    struct PointLight
    {
        Vec3f position;
        Vec3f intensity;
    };

    struct Material
    {
        Vec3f ambient;
        Vec3f diffuse;
        Vec3f specular;
        Vec3f mirror;
        float phong_exponent;
    };

    struct Face
    {
        int v0_id;
        int v1_id;
        int v2_id;
        Vec3f normal;
    };

    struct Mesh
    {
        int material_id;
        std::vector<Face> faces;

    };

    struct Triangle
    {
        int material_id;
        Face indices;
        Vec3f normalVec;
    };

    struct Sphere
    {
        int material_id;
        int center_vertex_id;
        float radius;
    };

    struct Scene
    {
        //Data
        Vec3i background_color;
        float shadow_ray_epsilon;
        int max_recursion_depth;
        std::vector<Camera> cameras;
        Vec3f ambient_light;
        std::vector<PointLight> point_lights;
        std::vector<Material> materials;
        std::vector<Vec3f> vertex_data;
        std::vector<Mesh> meshes;
        std::vector<Triangle> triangles;
        std::vector<Sphere> spheres;

        //Functions
        void loadFromXml(const std::string& filepath);
    };
}

#endif