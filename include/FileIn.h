#pragma once
#include "TriangleContainer.h"
//class Tri
//{
//public:
//	glm::vec3 A, B, C;
//};

bool ReadObjFile(std::ifstream& file, std::vector<TriangleContainer*>& triangles);
void PrintVec3(glm::vec3 &vec);
void PrintTriangle(TriangleContainer* tri);