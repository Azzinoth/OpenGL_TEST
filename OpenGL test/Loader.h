#pragma once
#include "RawModel.h"
#include "PNGTextureLoader.h"
#include <string>

struct OBJObject {
	std::vector<float> verticesArray;
	std::vector<float> texturesArray;
	std::vector<float> normalsArray;
	std::vector<int> indicesArray;

	std::string name;
};

class Loader {

	std::vector<GLuint> vaos;
	std::vector<GLuint> vbos;
	std::vector<GLuint> textures;

	GLuint createVAO() {
		GLuint vaoID;
		GL_ERROR(glGenVertexArrays(1, &vaoID));
		vaos.push_back(vaoID);
		GL_ERROR(glBindVertexArray(vaoID));
		return vaoID;
	}

	void storeDataInAttributeList(int attributeNumber, int coordinbateSize, std::vector<float> data) {
		GLuint vboID;
		GL_ERROR(glGenBuffers(1, &vboID));
		vbos.push_back(vboID);
		GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, vboID));
		GL_ERROR(glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW));
		GL_ERROR(glVertexAttribPointer(attributeNumber, coordinbateSize, GL_FLOAT, false, 0, 0));
		GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void unbindVAO() {
		GL_ERROR(glBindVertexArray(0));
	}

	void bindIndicesBuffer(std::vector<int> indices) {
		GLuint vboID;
		GL_ERROR(glGenBuffers(1, &vboID));
		vbos.push_back(vboID);
		GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID));
		GL_ERROR(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), indices.data(), GL_STATIC_DRAW));

	}

	inline void mySplit(char* text, const char* delimiters, std::vector<std::string>& dest) {
		dest.reserve(4);

		char* pch;
		pch = strtok(text, delimiters);
		while (pch != nullptr) {
			dest.push_back(pch);
			pch = strtok(nullptr, delimiters);
		}
	}

public:

	RawModel* loadToVAO(std::vector<float>& positions, std::vector<float>& textureCoords,
	std::vector<float>& normals, std::vector<int>& indices) {
		GLuint vaoID = createVAO();
		bindIndicesBuffer(indices);
		storeDataInAttributeList(0, 3, positions);
		storeDataInAttributeList(1, 2, textureCoords);
		storeDataInAttributeList(2, 3, normals);
		unbindVAO();
		
		return new RawModel(vaoID, indices.size());
	}

	RawModel* loadToVAO(std::vector<float>& positions) {
		GLuint vaoID = createVAO();
		storeDataInAttributeList(0, 2, positions);
		unbindVAO();

		return new RawModel(vaoID, positions.size() / 2);
	}

	RawModel* loadToVAO(std::string& requiredModel) {
		if (requiredModel == std::string("plane")) {
			std::vector<float> positions = { -0.5f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f };
			std::vector<int> indices = { 0, 1, 3, 3, 1, 2 };
			std::vector<float> textureCoords = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
			std::vector<float> normals = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };

			GLuint vaoID = createVAO();
			bindIndicesBuffer(indices);
			storeDataInAttributeList(0, 3, positions);
			storeDataInAttributeList(1, 2, textureCoords);
			storeDataInAttributeList(2, 3, normals);
			unbindVAO();

			return new RawModel(vaoID, indices.size());
		}

		return nullptr;
	}

	GLuint loadTexture(std::string fileName) {
		int w,h;
		GLuint texture = png_texture_load(fileName.c_str(), &w, &h);
		GL_ERROR(glGenerateMipmap(GL_TEXTURE_2D));
		GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GL_ERROR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.8f));

		textures.push_back(texture);
		return texture;
	}

	RawModel* loadFromOBJ(const char* fileName) {
		std::vector<OBJObject*> objects;

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> textures;
		std::vector<glm::vec3> normals;
		std::vector<int> indices;

		FILE* myFile;
		myFile = fopen(fileName, "r");

		if (!myFile) {
			fclose(myFile);
			return nullptr;
		}

		int vertexDisplacement = 0;
		int textureDisplacement = 0;
		int normalDisplacement = 0;
		int indexDisplacement = 0;

		int textureDisplacementCounter = 0;
		int normalDisplacementCounter = 0;
		int vertexDisplacementCounter = 0;

		std::vector<std::string> data;

		while (!feof(myFile)) {
			char buff[100];
			fgets(buff, 100, myFile);
			
			if  (buff[0] == 'o' && buff[1] == ' ') {
				if (objects.size() >= 1) {
					vertices.clear();
					textures.clear();
					normals.clear();
					indices.clear();
				}

				objects.push_back(new OBJObject());

				objects.back()->name = buff;
				objects.back()->name.erase(objects.back()->name.begin(), objects.back()->name.begin() + 2);
			}
			else if (buff[0] == 'v' && buff[1] == ' ') {
				char* line = new char[100];
				strcpy(line, buff);

				//std::vector<std::string> data;
				data.clear();
				mySplit(line, " ",data);

				vertices.push_back(glm::vec3(std::stof(data[1]), std::stof(data[2]), std::stof(data[3])));

				delete[] line;
			}
			else if (buff[0] == 'v' && buff[1] == 't') {
				char* line = new char[100];
				strcpy(line, buff);

				//std::vector<std::string> data;
				data.clear();
				mySplit(line, " ", data);

				textures.push_back(glm::vec2(std::stof(data[1]), std::stof(data[2])));

				delete[] line;
			}
			else if (buff[0] == 'v' && buff[1] == 'n') {
				char* line = new char[100];
				strcpy(line, buff);

				//std::vector<std::string> data;
				data.clear();
				mySplit(line, " ", data);

				normals.push_back(glm::vec3(std::stof(data[1]), std::stof(data[2]), std::stof(data[3])));

				delete[] line;
			}
			else if (buff[0] == 'f' && buff[1] == ' ') {
				if (objects.back()->texturesArray.size() == 0) {
					objects.back()->verticesArray.resize(vertices.size() * 3);
					objects.back()->texturesArray.resize(vertices.size() * 2);
					objects.back()->normalsArray.resize(vertices.size() * 3);
					
					int index = 0;

					for (auto ver : vertices) {
						objects.back()->verticesArray[index] = ver.x;
						objects.back()->verticesArray[index + 1] = ver.y;
						objects.back()->verticesArray[index + 2] = ver.z;

						index += 3;
					}

					textureDisplacement = textureDisplacementCounter;
					normalDisplacement = normalDisplacementCounter;
					vertexDisplacement = vertexDisplacementCounter;
				}

				char* line = new char[100];
				strcpy(line, buff);
				 
				std::vector<std::string> lineOfVertex;
				mySplit(line, " ", lineOfVertex);

				auto processVertex = [&](std::vector<std::string>& vertexData) {
					int currentPosition = (int)std::stof(vertexData[0]) - 1 - vertexDisplacement;
					objects.back()->indicesArray.push_back(currentPosition);

					objects.back()->texturesArray[(unsigned int)currentPosition * 2] = textures[(unsigned int)std::stof(vertexData[1]) - 1 - textureDisplacement].x;
					objects.back()->texturesArray[(unsigned int)currentPosition * 2 + 1] = 1 - textures[(unsigned int)std::stof(vertexData[1]) - 1 - textureDisplacement].y;

					objects.back()->normalsArray[(unsigned int)currentPosition * 3] = normals[(unsigned int)std::stof(vertexData[2]) - 1 - normalDisplacement].x;
					objects.back()->normalsArray[(unsigned int)currentPosition * 3 + 1] = normals[(unsigned int)std::stof(vertexData[2]) - 1 - normalDisplacement].y;
					objects.back()->normalsArray[(unsigned int)currentPosition * 3 + 2] = normals[(unsigned int)std::stof(vertexData[2]) - 1 - normalDisplacement].z;

					if (vertexDisplacementCounter < std::stof(vertexData[0])) vertexDisplacementCounter = std::stof(vertexData[0]);
					if (textureDisplacementCounter < std::stof(vertexData[1])) textureDisplacementCounter = std::stof(vertexData[1]);
					if (normalDisplacementCounter < std::stof(vertexData[2])) normalDisplacementCounter = std::stof(vertexData[2]);
				};

				std::vector<std::string> vertex1;
				mySplit((char*)lineOfVertex[1].c_str(), "/", vertex1);
				processVertex(vertex1);

				std::vector<std::string> vertex2;
				mySplit((char*)lineOfVertex[2].c_str(), "/", vertex2);
				processVertex(vertex2);

				std::vector<std::string> vertex3;
				mySplit((char*)lineOfVertex[3].c_str(), "/", vertex3);
				processVertex(vertex3);
			}
		}

		fclose(myFile);

		// Models with sub meshes
		//
		//

		//for (auto obj : objects) {
		//	modelVector.push_back(loadToVAO(obj->verticesArray, obj->texturesArray, obj->normalsArray, obj->indicesArray));
		//}

		// Models with sub meshes
		//
		//

		return loadToVAO(objects.front()->verticesArray, objects.front()->texturesArray, objects.front()->normalsArray, objects.front()->indicesArray);
	}

	void cleanUp() {
		GL_ERROR(glDeleteVertexArrays(vaos.size(), vaos.data()));
		GL_ERROR(glDeleteBuffers(vbos.size(), vbos.data()));
		GL_ERROR(glDeleteTextures(textures.size(), textures.data()));
	}

	~Loader() {
		cleanUp();
	}

};