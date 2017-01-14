#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <glm/glm.hpp>
#include "FileIn.h"
#include "Cube.h"

bool ReadObjFile(std::ifstream& file, std::vector<TriangleContainer*>& triangles)
{
	std::string str;
	std::vector<glm::vec3> vertices;
	Shape* shape = new Cube();
	// reserve something sensible
	vertices.reserve(50000);

	int lineCount = 0;

	while (getline(file, str))
	{
		if (!str.empty())
		{
			std::istringstream ss(str);
			std::string field;

			if (str[0] == 'v')
			{
				// discard v
				getline(ss, field, ' ');
				
				glm::vec3 newVertice;

				if (getline(ss, field, ' '))
				{
					std::stringstream fs(field);
					fs >> newVertice.x;
				}
				else
				{
					std::cout << "Error in line " << lineCount << ".\n";
					return false;
				}
				if (getline(ss, field, ' '))
				{
					std::stringstream fs(field);
					fs >> newVertice.y;
				}
				else
				{
					std::cout << "Error in line " << lineCount << ".\n";
					return false;
				}
				if (getline(ss, field, ' '))
				{
					std::stringstream fs(field);
					fs >> newVertice.z;
				}
				else
				{
					std::cout << "Error in line " << lineCount << ".\n";
					return false;
				}

				vertices.push_back(newVertice);
			}
			else if (str[0] == 'f')
			{
				// discard v
				getline(ss, field, ' ');

				//TriangleContainer* newTri = new TriangleContainer(,);
				glm::vec3 A, B, C;

				if (getline(ss, field, ' '))
				{
					std::stringstream fs(field);
					int vertexIndex = 0;
					fs >> vertexIndex;
					// index in file is not zero based
					if (--vertexIndex < vertices.size())
					{
						A = vertices[vertexIndex];
					}
					else
					{
						std::cout << "Error in line " << lineCount << ".\n";
						return false;
					}
				}
				else
				{
					std::cout << "Error in line " << lineCount << ".\n";
					return false;
				}
				if (getline(ss, field, ' '))
				{
					std::stringstream fs(field);
					int vertexIndex = 0;
					fs >> vertexIndex;
					if (--vertexIndex < vertices.size())
					{
						B = vertices[vertexIndex];
					}
					else
					{
						std::cout << "Error in line " << lineCount << ".\n";
						return false;
					}
				}
				else
				{
					std::cout << "Error in line " << lineCount << ".\n";
					return false;
				}
				if (getline(ss, field, ' '))
				{
					std::stringstream fs(field);
					int vertexIndex = 0;
					fs >> vertexIndex;
					if (--vertexIndex < vertices.size())
					{
						C = vertices[vertexIndex];
					}
					else
					{
						std::cout << "Error in line " << lineCount << ".\n";
						return false;
					}
				}
				else
				{
					std::cout << "Error in line " << lineCount << ".\n";
					return false;
				}

				triangles.push_back(new TriangleContainer(A, B, C, shape));
			}
		}
		lineCount++;
	}
	return true;
}

void PrintVec3(glm::vec3 &vec)
{
	std::cout << "(";
	std::cout << vec.x;
	std::cout << ",";
	std::cout << vec.y;
	std::cout << ",";
	std::cout << vec.z;
	std::cout << ")";
}

void PrintTri(TriangleContainer* tri)
{
	std::cout << "(";
	PrintVec3(tri->world_A);
	std::cout << ",";
	PrintVec3(tri->world_B);
	std::cout << ",";
	PrintVec3(tri->world_C);
	std::cout << ")";
}