#include "mesh_tools.h"

#include <3d/CCMesh.h>
#include <base/CCDirector.h>
#include <renderer/CCTextureCache.h>
#include <renderer/CCMaterial.h>

USING_NS_CC;

namespace Editor
{

    struct V3F_N3F
    {
        Vec3 position;
        Vec3 normal;
    };

    struct VertexXYZN
    {
        float x, y, z;
        float nx, ny, nz;
    };

    
    Sprite3D* createSquareModel(float width, float height, const std::string & texture, float repeatStep, const std::string &shader)
    {
        float halfWidth = width * 0.5f;
        float halfHeight = height * 0.5f;

        float uvScaleX = 1.0f;
        float uvScaleY = 1.0f;
        if(repeatStep > 0.0f)
        {
            uvScaleX = halfWidth / repeatStep;
            uvScaleY = halfHeight / repeatStep;
        }
        
        /* create quad vertices.
         * 2 - 3
         * | + |
         * 0 - 1
         * NOTICE: the vertical texture coords is inverted in shader.
         */
        std::vector<float> vertices(sizeof(V3F_T2F_Quad) / sizeof(float));
        V3F_T2F_Quad *quad = (V3F_T2F_Quad *)vertices.data();
        quad->tl.vertices = Vec3(-halfWidth, 0.0f, -halfHeight);
        quad->tl.texCoords = Tex2F(0, uvScaleY);
        
        quad->tr.vertices = Vec3(halfWidth, 0.0f, -halfHeight);
        quad->tr.texCoords = Tex2F(uvScaleX, uvScaleY);
        
        quad->bl.vertices = Vec3(-halfWidth, 0.0f, halfHeight);
        quad->bl.texCoords = Tex2F(0, 0);
        
        quad->br.vertices = Vec3(halfWidth, 0.0f, halfHeight);
        quad->br.texCoords = Tex2F(uvScaleX, 0);
        
        // generate vertex indices. in counterclockwise.
        Mesh::IndexArray indices = {0, 1, 2, 3, 2, 1};
        
        // generate vertex attributes.
        std::vector<MeshVertexAttrib> attribs;
        MeshVertexAttrib attr;
        attr.size = 3;
        attr.type = GL_FLOAT;
        attr.vertexAttrib = GLProgram::VERTEX_ATTRIB_POSITION;
        attr.attribSizeBytes = sizeof(Vec3);
        attribs.push_back(attr);
        
        attr.size = 2;
        attr.type = GL_FLOAT;
        attr.vertexAttrib = GLProgram::VERTEX_ATTRIB_TEX_COORD;
        attr.attribSizeBytes = sizeof(Tex2F);
        attribs.push_back(attr);
        
        // create mesh.
        Mesh *mesh = Mesh::create(vertices, sizeof(V3F_T2F_Quad) / sizeof(float), indices, attribs);
        Texture2D *tex = Director::getInstance()->getTextureCache()->addImage(texture);
        if(tex)
        {
            if(repeatStep > 0.0f)
            {
                tex->generateMipmap();

                Texture2D::TexParams param;
                param.minFilter = GL_LINEAR_MIPMAP_LINEAR;
                param.magFilter = GL_LINEAR;
                param.wrapS = GL_REPEAT;
                param.wrapT = GL_REPEAT;

                tex->setTexParameters(param);
            }
            mesh->setTexture(tex);
        }
        GLProgramState *glProgram = GLProgramState::getOrCreateWithGLProgramName(shader);
        mesh->setGLProgramState(glProgram);
        
        Sprite3D *model = Sprite3D::create();
        model->addMesh(mesh);
        
        return model;
    }

    cocos2d::Sprite3D* createGridModel(int rows, int cols,
                                       float gridWidth, float gridHeight,
                                       const std::string &texture)
    {
        int rowVertices = rows + 1;
        int colVertices = cols + 1;

        float halfWidth = cols * gridWidth * 0.5f;
        float halfHeight = rows * gridHeight * 0.5f;

        typedef V3F_T2F VertexType;

        /* grid order
         * (0,1) | (1,1)
         * -     -     - x+
         * (0,0) | (0,1)
         *       z+
        */

        // generate vertices.
        std::vector<float> vertices(rowVertices * colVertices * (sizeof(VertexType) / sizeof(float)));
        VertexType *pv = (VertexType*)vertices.data();
        for(int r = 0; r < rowVertices; ++r)
        {
            for(int c = 0; c < colVertices; ++c)
            {
                pv->vertices.x = c * gridWidth - halfWidth;
                pv->vertices.y = 0.0f;
                pv->vertices.z = halfHeight - r * gridHeight;

                pv->texCoords.u = c;
                pv->texCoords.v = r;

                ++pv;
            }
        }

        // generate indices.
        Mesh::IndexArray indices(rows * cols * 2 * 3);
        Mesh::IndexArray::value_type *pi = indices.data();
        for(int r = 0; r < rows; ++r)
        {
            for(int c = 0; c < cols; ++c)
            {
                /* index order.
                 * p2 -> p3
                 * p0 -> p1
                */
                pi[0] = r * colVertices + c;
                pi[1] = pi[0] + 1;
                pi[2] = pi[0] + colVertices;

                pi[3] = pi[2] + 1;
                pi[4] = pi[2];
                pi[5] = pi[1];

                pi += 6;
            }
        }

        // generate vertex attributes.
        std::vector<MeshVertexAttrib> attribs;
        MeshVertexAttrib attr;
        attr.size = 3;
        attr.type = GL_FLOAT;
        attr.vertexAttrib = GLProgram::VERTEX_ATTRIB_POSITION;
        attr.attribSizeBytes = sizeof(Vec3);
        attribs.push_back(attr);

        attr.size = 2;
        attr.type = GL_FLOAT;
        attr.vertexAttrib = GLProgram::VERTEX_ATTRIB_TEX_COORD;
        attr.attribSizeBytes = sizeof(Tex2F);
        attribs.push_back(attr);

        // create mesh.
        Mesh *mesh = Mesh::create(vertices, sizeof(VertexType) / sizeof(float), indices, attribs);
        if(!texture.empty())
        {
            Texture2D *tex = Director::getInstance()->getTextureCache()->addImage(texture);
            if(tex != nullptr)
            {
                tex->generateMipmap();

                Texture2D::TexParams param;
                param.minFilter = GL_LINEAR_MIPMAP_LINEAR;
                param.magFilter = GL_LINEAR;
                param.wrapS = GL_REPEAT;
                param.wrapT = GL_REPEAT;

                tex->setTexParameters(param);
                mesh->setTexture(tex);
            }
        }

        GLProgramState *glProgram = GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_3D_POSITION_TEXTURE);
        mesh->setGLProgramState(glProgram);

        Sprite3D *model = Sprite3D::create();
        model->addMesh(mesh);
        return model;
    }

    /* a,b,c,d in counterclockwise(CCW):
     * 3 - 2
     * |   |
     * 0 - 1
    */
    static void generateFaceVertices(V3F_N3F *vertices, int &offset, Mesh::IndexArray &indices, const Vec3 *positions, int a, int b, int c, int d)
    {
        vertices[offset + 0].position = positions[a];
        vertices[offset + 1].position = positions[b];
        vertices[offset + 2].position = positions[c];
        vertices[offset + 3].position = positions[d];

        indices.push_back(offset + 0);
        indices.push_back(offset + 1);
        indices.push_back(offset + 3);

        indices.push_back(offset + 2);
        indices.push_back(offset + 3);
        indices.push_back(offset + 1);

        Vec3 normal;
        Vec3::cross(positions[b] - positions[a], positions[c] - positions[a], &normal);
        normal.normalize();

        for(int i = offset; i < offset + 4; ++i)
        {
            vertices[i].normal = normal;
        }

        offset += 4;
    }

    cocos2d::Sprite3D* createCube(const cocos2d::Vec3 &radius, const cocos2d::Color4B &color)
    {
        typedef V3F_N3F VertexType;

        /*
         *   6 - 7
         *  /|  /|
         * 2 - 3 |
         * | 4-|-5
         * |/  |/
         * 0 - 1
        */

        Vec3 positions[] = {
            {-radius.x, -radius.y, radius.z},
            {radius.x, -radius.y, radius.z},
            {-radius.x, radius.y, radius.z},
            {radius.x, radius.y, radius.z},
            {-radius.x, -radius.y, -radius.z},
            {radius.x, -radius.y, -radius.z},
            {-radius.x, radius.y, -radius.z},
            {radius.x, radius.y, -radius.z},
        };

        // generate vertices.
        std::vector<float> vertices(24 * sizeof(VertexType) / sizeof(float));
        VertexType *pv = (VertexType*)vertices.data();
        Mesh::IndexArray indices;

        int offset = 0;
        generateFaceVertices(pv, offset, indices, positions, 0, 1, 3, 2); //front
        generateFaceVertices(pv, offset, indices, positions, 4, 6, 7, 5); //back
        generateFaceVertices(pv, offset, indices, positions, 0, 2, 6, 4); //left
        generateFaceVertices(pv, offset, indices, positions, 1, 5, 7, 3); //right
        generateFaceVertices(pv, offset, indices, positions, 2, 3, 7, 6); //top
        generateFaceVertices(pv, offset, indices, positions, 0, 4, 5, 1); //bottom

        // generate vertex attributes.
        std::vector<MeshVertexAttrib> attribs;
        MeshVertexAttrib attr;
        attr.size = 3;
        attr.type = GL_FLOAT;
        attr.vertexAttrib = GLProgram::VERTEX_ATTRIB_POSITION;
        attr.attribSizeBytes = sizeof(Vec3);
        attribs.push_back(attr);

        attr.size = 3;
        attr.type = GL_FLOAT;
        attr.vertexAttrib = GLProgram::VERTEX_ATTRIB_NORMAL;
        attr.attribSizeBytes = sizeof(Vec3);
        attribs.push_back(attr);

        // create mesh.
        Mesh *mesh = Mesh::create(vertices, sizeof(VertexType) / sizeof(float), indices, attribs);

        GLProgramState *glProgram = GLProgramState::getOrCreateWithGLProgramName(GLProgram::SHADER_3D_POSITION_NORMAL);
        mesh->setGLProgramState(glProgram);

        Sprite3D *model = Sprite3D::create();
        model->addMesh(mesh);
        model->setColor(Color3B(color));
        model->setOpacity(color.a);
        return model;
    }
}
