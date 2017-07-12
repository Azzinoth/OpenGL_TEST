#pragma once

#include "RawModel.h"
#include "PNGTextureLoader.h"
#include "Checker.h"
#include <string>

struct OBJObject {
	std::vector<float> verticesArray;
	std::vector<float> texturesArray;
	std::vector<float> normalsArray;
	std::vector<float> tangentsArray;
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

	glm::vec3 calculateTangents(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2,
		std::vector<glm::vec2>&& textures) {
		/*glm::vec3 delatPos1 = v1 - v0;
		glm::vec3 delatPos2 = v2 - v0;
		glm::vec2 uv0 = textures[0];
		glm::vec2 uv1 = textures[1];
		glm::vec2 uv2 = textures[2];
		glm::vec2 deltaUv1 = uv1 - uv0;
		glm::vec2 deltaUv2 = uv2 - uv0;

		float r = 1.0f / (deltaUv1.x * deltaUv2.y - deltaUv1.y * deltaUv2.x);
		delatPos1 = delatPos1 * deltaUv2.y;
		delatPos2 = delatPos2 * deltaUv1.y;
		glm::vec3 tangent = delatPos1 - delatPos2;
		tangent = tangent * r;

		if (tangent.x == -NAN) {
			assert(0);
		}*/

		glm::vec3 q1 = v1 - v0;
		glm::vec3 q2 = v2 - v0;
		glm::vec2 uv0 = textures[0];
		glm::vec2 uv1 = textures[1];
		glm::vec2 uv2 = textures[2];

		float t1 = uv1.y - uv0.y;
		float t2 = uv2.y - uv0.y;

		glm::vec3 tangent = t2*q1 - t1*q2;

		return tangent;
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

	RawModel* loadToVAO(std::vector<float>& positions, std::vector<float>& textureCoords,
		std::vector<float>& normals, std::vector<float>& tangents, std::vector<int>& indices) {
		GLuint vaoID = createVAO();
		bindIndicesBuffer(indices);
		storeDataInAttributeList(0, 3, positions);
		storeDataInAttributeList(1, 2, textureCoords);
		storeDataInAttributeList(2, 3, normals);
		storeDataInAttributeList(3, 3, tangents);
		unbindVAO();

		return new RawModel(vaoID, indices.size());
	}

	RawModel* loadToVAO(std::vector<float>& positions, int dimensions = 2) {
		GLuint vaoID = createVAO();
		storeDataInAttributeList(0, dimensions, positions);
		unbindVAO();

		return new RawModel(vaoID, positions.size() / dimensions);
	}

	RawModel* loadToVAO(std::string& requiredModel) {
		if (requiredModel == std::string("plane")) {
			std::vector<float> positions = { -0.5f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f };
			std::vector<int> indices = { 0, 1, 2, 3, 0, 2 };
			std::vector<float> textureCoords = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f };
			std::vector<float> normals = { 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };


			glm::vec3 tangent0 = calculateTangents(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f), { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f) });
			glm::vec3 tangent1 = calculateTangents(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.5f, -0.5f, 0.0f), { glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f) });


			std::vector<float> tangents = { tangent0.x, tangent0.y, tangent0.z, tangent0.x, tangent0.y, tangent0.z, tangent0.x, tangent0.y, tangent0.z, tangent1.x, tangent1.y, tangent1.z };

			GLuint vaoID = createVAO();
			bindIndicesBuffer(indices);
			storeDataInAttributeList(0, 3, positions);
			storeDataInAttributeList(1, 2, textureCoords);
			storeDataInAttributeList(2, 3, normals);
			storeDataInAttributeList(3, 3, tangents);
			unbindVAO();

			return new RawModel(vaoID, indices.size());
		}

		return nullptr;
	}

	GLuint loadTexture(std::string fileName) {
		GLuint texture = png_texture_load(fileName.c_str());
		GL_ERROR(glGenerateMipmap(GL_TEXTURE_2D));
		GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR)); // smooth textures not blocky :)
		GL_ERROR(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.8f));

		textures.push_back(texture);
		return texture;
	}

	std::vector<unsigned char> loadTextureData(std::string fileName, int& w, int& h) {
		return png_texture_loadData(fileName.c_str(), w, h);;
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

		glm::vec3 minVertex = glm::vec3(0.0);
		glm::vec3 maxVertex = glm::vec3(0.0);

		std::vector<std::string> data;
		objects.push_back(new OBJObject());

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

				//objects.push_back(new OBJObject());

				objects.back()->name = buff;
				objects.back()->name.erase(objects.back()->name.begin(), objects.back()->name.begin() + 2);
			}
			else if (buff[0] == 'v' && buff[1] == ' ') {
				char* line = new char[100];
				strcpy(line, buff);

				//std::vector<std::string> data;
				data.clear();
				mySplit(line, " ",data);

				glm::vec3 currentVertex = { std::stof(data[1]), std::stof(data[2]), std::stof(data[3]) };
				vertices.push_back(currentVertex);

				// looking for min, max of boundingbox.
				if (vertices.size() == 3) {
					minVertex = currentVertex;
					maxVertex = currentVertex;
				}

				if (minVertex.x > currentVertex.x) minVertex.x = currentVertex.x;
				if (minVertex.y > currentVertex.y) minVertex.y = currentVertex.y;
				if (minVertex.z > currentVertex.z) minVertex.z = currentVertex.z;

				if (maxVertex.x < currentVertex.x) maxVertex.x = currentVertex.x;
				if (maxVertex.y < currentVertex.y) maxVertex.y = currentVertex.y;
				if (maxVertex.z < currentVertex.z) maxVertex.z = currentVertex.z;

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
					objects.back()->tangentsArray.resize(vertices.size() * 3);
					
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

					//if (objects.back()->indicesArray.size() % 3 == 0) {
						//
					//}
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

		// calculateTangents
		for (int i = 0; i < objects.back()->indicesArray.size() - 1; i += 3) {
			glm::vec3 v0 = { objects.back()->verticesArray[objects.back()->indicesArray[i] * 3], objects.back()->verticesArray[objects.back()->indicesArray[i] * 3 + 1], objects.back()->verticesArray[objects.back()->indicesArray[i] * 3 + 2] };
			glm::vec3 v1 = { objects.back()->verticesArray[objects.back()->indicesArray[i + 1] * 3], objects.back()->verticesArray[objects.back()->indicesArray[i + 1] * 3 + 1], objects.back()->verticesArray[objects.back()->indicesArray[i + 1] * 3 + 2] };
			glm::vec3 v2 = { objects.back()->verticesArray[objects.back()->indicesArray[i + 2] * 3], objects.back()->verticesArray[objects.back()->indicesArray[i + 2] * 3 + 1], objects.back()->verticesArray[objects.back()->indicesArray[i + 2] * 3 + 2] };

			glm::vec2 t0 = { objects.back()->texturesArray[objects.back()->indicesArray[i] * 2], objects.back()->texturesArray[objects.back()->indicesArray[i] * 2 + 1] };
			glm::vec2 t1 = { objects.back()->texturesArray[objects.back()->indicesArray[i + 1] * 2], objects.back()->texturesArray[objects.back()->indicesArray[i + 1] * 2 + 1] };
			glm::vec2 t2 = { objects.back()->texturesArray[objects.back()->indicesArray[i + 2] * 2], objects.back()->texturesArray[objects.back()->indicesArray[i + 2] * 2 + 1] };

			glm::vec3 tangent = calculateTangents(v0, v1, v2, { t0, t1, t2 });
			tangent = glm::normalize(tangent);

			objects.back()->tangentsArray[objects.back()->indicesArray[i] * 3] = tangent.x;
			objects.back()->tangentsArray[objects.back()->indicesArray[i] * 3 + 1] = tangent.y;
			objects.back()->tangentsArray[objects.back()->indicesArray[i] * 3 + 2] = tangent.z;

			objects.back()->tangentsArray[objects.back()->indicesArray[i + 1] * 3] = tangent.x;
			objects.back()->tangentsArray[objects.back()->indicesArray[i + 1] * 3 + 1] = tangent.y;
			objects.back()->tangentsArray[objects.back()->indicesArray[i + 1] * 3 + 2] = tangent.z;

			objects.back()->tangentsArray[objects.back()->indicesArray[i + 2] * 3] = tangent.x;
			objects.back()->tangentsArray[objects.back()->indicesArray[i + 2] * 3 + 1] = tangent.y;
			objects.back()->tangentsArray[objects.back()->indicesArray[i + 2] * 3 + 2] = tangent.z;
		}

		
		// Models with sub meshes
		//
		//

		//for (auto obj : objects) {
		//	modelVector.push_back(loadToVAO(obj->verticesArray, obj->texturesArray, obj->normalsArray, obj->indicesArray));
		//}

		// Models with sub meshes
		//
		//

		//RawModel* model = loadToVAO(objects.front()->verticesArray, objects.front()->texturesArray, objects.front()->normalsArray, objects.front()->indicesArray);
		RawModel* model = loadToVAO(objects.front()->verticesArray, objects.front()->texturesArray, objects.front()->normalsArray, objects.front()->tangentsArray, objects.front()->indicesArray);
		model->setBoundingboxMin(minVertex);
		model->setBoundingboxMax(maxVertex);

		return model;
	}

	GLuint loadCubeMap(std::vector<std::string>& textureFiles) {
		GLuint texID;
		GL_ERROR(glGenTextures(1, &texID));
		GL_ERROR(glActiveTexture(GL_TEXTURE0));
		GL_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, texID));

		for (int i = 0; i < textureFiles.size(); i++) {
			int w = 0, h = 0;
			std::vector<unsigned char> data = png_texture_loadData(textureFiles[i].c_str(), w, h);
			GL_ERROR(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data()));
		}

		GL_ERROR(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_ERROR(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

		// Due to hardware limitations on some computers you may see some visible seams at the edges of the skybox. 
		// If this is the case then add these two lines to the end of the loadCubeMap() method, just before returning the texID
		GL_ERROR(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GL_ERROR(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		textures.push_back(texID);
		return texID;
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