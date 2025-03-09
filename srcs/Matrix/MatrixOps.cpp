#include "Matrix/Matrix.hpp"
#include "Utils/Utils.hpp"

namespace AlgOps
{
    mat4 rotate(const mat4 &instance, float angle, const vec3 &vector)
    {
        mat4 result = instance;
        vec3 vectorNormalized = normalize(vector);
        
        float cosAngle = cosf(angle);
        float sinAngle = sinf(angle);
        result = {
            cosAngle + powf(vectorNormalized.getX(), 2) * (1 - cosAngle),
            vectorNormalized.getX() * vectorNormalized.getY() * (1 - cosAngle) - vectorNormalized.getZ() * sinAngle,
            vectorNormalized.getX() * vectorNormalized.getZ() * (1 - cosAngle) + vectorNormalized.getY() * sinAngle, 0,
            // new line
            vectorNormalized.getY() * vectorNormalized.getX() * (1 - cosAngle) + vectorNormalized.getZ() * sinAngle,
            cosAngle + powf(vectorNormalized.getY(), 2) * (1 - cosAngle),
            vectorNormalized.getY() * vectorNormalized.getZ() * (1 - cosAngle) - vectorNormalized.getX() * sinAngle, 0,
            // new line
            vectorNormalized.getZ() * vectorNormalized.getX() * (1 - cosAngle) - vectorNormalized.getY() * sinAngle,
            vectorNormalized.getZ() * vectorNormalized.getY() * (1 - cosAngle) + vectorNormalized.getX() * sinAngle,
            cosAngle + powf(vectorNormalized.getZ(), 2) * (1 - cosAngle), 0,
            // new line
            0, 0, 0, 1
        };
        return (result);
    }

    AlgOps::mat4 perspective(float fov, float aspect, float near, float far)
    {
        AlgOps::mat4 result;
        
        float tangent = tanf(Utils::DegToRad(fov / 2));
        float halfHeight = near * tangent;
        float halfWidth = halfHeight * aspect;
        
        float left = halfWidth;
        float right = -halfWidth;
        float top = halfHeight;
        float bottom = -halfHeight;
        
        /*
        modification :
        add a minus for (0, 0)
        swap (2, 3) and (3, 2)
        */
        result = {-((2 * near) / (right - left)), 0, (right + left) / (right - left), 0,
            // new line
            0, (2 * near) / (top - bottom), (top + bottom) / (top - bottom), 0,
            // new line
            0, 0, (-(far + near)) / (far - near), -1,
            // new line
            0, 0, (-2 * far * near) / (far - near), 0};
        return (result);
    }
        
    vec3 normalize(const vec3 &vector)
    {
        vec3 result = vector;
        float length;
        
        length = sqrt(powf(vector.getX(), 2) + powf(vector.getY(), 2) + powf(vector.getZ(), 2));
        if (length != 0)
            result = {vector.getX() / length, vector.getY() / length, vector.getZ() / length};
        return (result);
    }

    vec3 crossProduct(const vec3 &vectorA, const vec3 &vectorB)
    {
        vec3 result;
        
        float pointI[] = {vectorA.getX(), vectorA.getY(), vectorA.getZ()};

        float pointJ[] = {vectorB.getX(), vectorB.getY(), vectorB.getZ()};

        float vectorIJ[] = {pointJ[0] - pointI[0], pointJ[1] - pointI[1], pointJ[2] - pointI[2]};

        result = {vectorA.getY() * vectorIJ[2] - vectorA.getZ() * vectorIJ[1],
                    vectorA.getZ() * vectorIJ[0] - vectorA.getX() * vectorIJ[2],
                    vectorA.getX() * vectorIJ[1] - vectorA.getY() * vectorIJ[0]};
        return (result);
    }

    float dotProduct(const vec3 &vectorA, const vec3 &vectorB)
    {
        return (vectorA.getX() * vectorB.getX() + vectorA.getY() * vectorB.getY() + vectorA.getZ() * vectorB.getZ());
    }

    mat4 lookAt(const vec3 &position, const vec3 &target, const vec3 &initialUpVector)
    {
        Matrix front(normalize(position - target));
        Matrix right(normalize(crossProduct(initialUpVector, front)));
        Matrix up(normalize(crossProduct(front, right)));
        vec3 translation;
        mat4 result;
        
        translation = {dotProduct(position, right), dotProduct(position, up), dotProduct(position, front)};
        
        result = {
            right.getX(), up.getX(), front.getX(), 0, right.getY(),        up.getY(),           front.getY(),        0,
            right.getZ(), up.getZ(), front.getZ(), 0, -translation.getX(), -translation.getY(), -translation.getZ(), 1};
        return (result);
    }

    mat4 translate(const mat4 &instance, const vec3 &vector)
    {
        mat4 result = instance;

        for (int i = 0; i < 3; i++)
            result.setData(3, i, result.getData(3, i) + vector.getData(i, 0)); // should be pos i, 3
		
        return result;
    }

    mat4 scale(const mat4 &instance, const vec3 &vector)
    {
        mat4 result = instance;

        for (int i = 0; i < 3; i++)
            result.setData(i, i, result.getData(i, i) * vector.getData(i, 0));
		
        return result;
    }
}