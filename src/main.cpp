#include <Windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include <memory>

namespace
{
    const char* VERTEX_SHADER_SOURCE = R"glsl(

#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 tex_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 o_color;
out vec2 o_tex_coord;

void main()
{
    gl_Position = proj * view * model * vec4(pos, 1.0);
    o_color = color;
    o_tex_coord = tex_coord;
}

    )glsl";

    const char* FRAGMENT_SHADER_SOURCE = R"glsl(

#version 330 core

in vec4 o_color;
in vec2 o_tex_coord;

uniform sampler2D tex;

out vec4 frag_color;

void main()
{
    frag_color = o_color * texture(tex, o_tex_coord);
}

    )glsl";

    struct vertex
    {
        float x;
        float y;
        float z;

        float r;
        float g;
        float b;
        float a;

        float u;
        float v;
    };

    const vertex VERTICES[] =
    {
        -1, -1, -1, 1, 0, 1, 1, 0, 0,
         1,  1, -1, 1, 0, 1, 1, 1, 1,
         1, -1, -1, 1, 0, 1, 1, 1, 0,
        -1,  1, -1, 1, 0, 1, 1, 0, 1,

        -1, -1,  1, 1, 0, 0, 1, 0, 0,
         1,  1,  1, 1, 0, 0, 1, 1, 1,
         1, -1,  1, 1, 0, 0, 1, 1, 0,
        -1,  1,  1, 1, 0, 0, 1, 0, 1,

        -1, -1, -1, 0, 1, 0, 1, 0, 0,
        -1,  1,  1, 0, 1, 0, 1, 1, 1,
        -1, -1,  1, 0, 1, 0, 1, 0, 1,
        -1,  1, -1, 0, 1, 0, 1, 1, 0,

         1, -1, -1, 0, 0, 1, 1, 0, 0,
         1,  1,  1, 0, 0, 1, 1, 1, 1,
         1, -1,  1, 0, 0, 1, 1, 0, 1,
         1,  1, -1, 0, 0, 1, 1, 1, 0,

        -1, -1, -1, 1, 1, 0, 1, 0, 0,
         1, -1,  1, 1, 1, 0, 1, 1, 1,
        -1, -1,  1, 1, 1, 0, 1, 0, 1,
         1, -1, -1, 1, 1, 0, 1, 1, 0,

        -1,  1, -1, 0, 1, 1, 1, 0, 0,
         1,  1,  1, 0, 1, 1, 1, 1, 1,
        -1,  1,  1, 0, 1, 1, 1, 0, 1,
         1,  1, -1, 0, 1, 1, 1, 1, 0,
    };

    unsigned int INDICES[] =
    {
        0, 1, 2,
        0, 3, 1,

        4, 6, 5,
        4, 5, 7,

        8, 10, 9,
        8, 9, 11,

        12, 13, 14,
        12, 15, 13,

        16, 17, 18,
        16, 19, 17,

        20, 22, 21,
        20, 21, 23,
    };

    int screen_width = 640;
    int screen_height = 480;
    int fb_width = 640;
    int fb_height = 480;

    void on_framebuffer_size(GLFWwindow* window, int width, int height)
    {
        fb_width = width;
        fb_height = height;

        glViewport(0, 0, width, height);
    }

    GLuint compile_shader(GLenum type, const char* source)
    {
        auto shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        int success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            char info_log[512] = {};
            glGetShaderInfoLog(shader, 512, nullptr, info_log);
            MessageBoxA(nullptr, info_log, "Error", MB_ICONERROR);
            return 0;
        }

        return shader;
    }
}

extern int WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    atexit(glfwTerminate);
    if (!glfwInit())
    {
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    auto window = std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>(glfwCreateWindow(screen_width, screen_height, "Minimal GL", nullptr, nullptr), glfwDestroyWindow);
    if (!window)
    {
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window.get());

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return EXIT_FAILURE;
    }

    glfwSetFramebufferSizeCallback(window.get(), on_framebuffer_size);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    auto vertex_shader = compile_shader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
    if (!vertex_shader)
    {
        return EXIT_FAILURE;
    }

    auto fragment_shader = compile_shader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);
    if (!fragment_shader)
    {
        return EXIT_FAILURE;
    }

    auto shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    int success = 0;
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char info_log[512] = {};
        glGetProgramInfoLog(shader_program, 512, nullptr, info_log);

        MessageBox(nullptr, info_log, "Error", MB_ICONERROR);
        return EXIT_FAILURE;
    }

    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, x));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, r));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, u));
    glEnableVertexAttribArray(2);

    GLuint ebo = 0;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES), INDICES, GL_STATIC_DRAW);

    int tex_width = 0;
    int tex_height = 0;
    int tex_channels = 0;
    uint8_t* data = stbi_load("Grass_01.png", &tex_width, &tex_height, &tex_channels, 4);
    if (!data)
    {
        MessageBoxA(nullptr, "Failed to load texture data", "Error", MB_ICONERROR);
        return EXIT_FAILURE;
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    while (!glfwWindowShouldClose(window.get()))
    {
        glfwPollEvents();

        glClearColor(0.39f, 0.58f, 0.92f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) fb_width / (float) fb_height, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

        glm::mat4 view = glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, -5));
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        float angle = glfwGetTime() * 50;

        glm::mat4 model = glm::identity<glm::mat4>();
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0, 0, 1));
        model = glm::rotate(model, glm::radians(angle / 2.0f), glm::vec3(0, 1, 0));
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shader_program, "tex"), 0);
        
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window.get());
    }

    return EXIT_SUCCESS;
}

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
