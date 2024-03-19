#include "scene0/skybox.h"
#include "helper/stb/stb_image.h"
#include <iostream>

void Skybox::Init() {
    float skyboxVertices[] = {
        // positions
        -1.0f, 1.0f,  -1.0f,  //
        -1.0f, -1.0f, -1.0f,  //
        1.0f,  -1.0f, -1.0f,  //
        1.0f,  -1.0f, -1.0f,  //
        1.0f,  1.0f,  -1.0f,  //
        -1.0f, 1.0f,  -1.0f,  //

        -1.0f, -1.0f, 1.0f,   //
        -1.0f, -1.0f, -1.0f,  //
        -1.0f, 1.0f,  -1.0f,  //
        -1.0f, 1.0f,  -1.0f,  //
        -1.0f, 1.0f,  1.0f,   //
        -1.0f, -1.0f, 1.0f,   //

        1.0f,  -1.0f, -1.0f,  //
        1.0f,  -1.0f, 1.0f,   //
        1.0f,  1.0f,  1.0f,   //
        1.0f,  1.0f,  1.0f,   //
        1.0f,  1.0f,  -1.0f,  //
        1.0f,  -1.0f, -1.0f,  //

        -1.0f, -1.0f, 1.0f,  //
        -1.0f, 1.0f,  1.0f,  //
        1.0f,  1.0f,  1.0f,  //
        1.0f,  1.0f,  1.0f,  //
        1.0f,  -1.0f, 1.0f,  //
        -1.0f, -1.0f, 1.0f,  //

        -1.0f, 1.0f,  -1.0f,  //
        1.0f,  1.0f,  -1.0f,  //
        1.0f,  1.0f,  1.0f,   //
        1.0f,  1.0f,  1.0f,   //
        -1.0f, 1.0f,  1.0f,   //
        -1.0f, 1.0f,  -1.0f,  //

        -1.0f, -1.0f, -1.0f,  //
        -1.0f, -1.0f, 1.0f,   //
        1.0f,  -1.0f, -1.0f,  //
        1.0f,  -1.0f, -1.0f,  //
        -1.0f, -1.0f, 1.0f,   //
        1.0f,  -1.0f, 1.0f    //
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);


    //crreate cubemap
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

	stbi_set_flip_vertically_on_load(false);

    int channels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &channels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}
void Skybox::Draw() {
    glBindVertexArray(vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}