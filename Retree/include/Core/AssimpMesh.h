#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>


#define MAX_BONE_INFLUENCE 4

using glm::vec3;
using glm::vec2;
using std::string;
using std::vector;

struct AssimpVertex {
    vec3 Position;
    vec3 Normal;
    vec2 TexCoords;
    vec3 Tangent;
    vec3 Bitangent;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct AssimpTexture {
    unsigned int id;
    string type;
    string path;
};

class Shader;

class AssimpMesh {
public:
    // mesh Data
    vector<AssimpVertex>       vertices;
    vector<unsigned int>       indices;
    vector<AssimpTexture>      textures;
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    // constructor
    AssimpMesh(
        vector<AssimpVertex> vertices, 
        vector<unsigned int> indices, 
        vector<AssimpTexture> textures
    );

    // render the mesh
    void Draw(Shader& shader);

private:
    
    void setupMesh();       // initializes all the buffer objects/arrays
};