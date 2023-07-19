#include "texture.hpp"
#include "integer.hpp"

#include <stb_image/stb_image.h>

Texture load_texture_from_file(const char* filename)
{
    Texture texture;

    // Read bytes from file
    byte* imageData = stbi_load(filename, &texture.width, &texture.height, NULL, 4);
    if (imageData == NULL)
    {
        return texture;
    }

    // Create a OpenGL texture identifier
    glGenTextures(1, &texture.data);
    glBindTexture(GL_TEXTURE_2D, texture.data);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, imageData);
    stbi_image_free(imageData);

    return texture;
}

Texture load_texture_from_memory()
{
    Texture texture;

    // Read bytes from file
    texture.width = 50;
    texture.height = 50;
    byte* imageData = stbi_load_from_memory(&trashcanBytes[0], sizeof(trashcanBytes), &texture.width, &texture.height, NULL, 4);

    // Create a OpenGL texture identifier
    glGenTextures(1, &texture.data);
    glBindTexture(GL_TEXTURE_2D, texture.data);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, imageData);

    return texture;
}
