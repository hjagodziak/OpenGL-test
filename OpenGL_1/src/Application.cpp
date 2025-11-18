#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);


    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile" << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
    std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1280, 960, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error!" << std::endl;
    }

    float positions[] = {
         -1.0f, -1.0f,
          1.0f, -1.0f,
          1.0f,  1.0f,

          1.0f,  1.0f,
         -1.0f,  1.0f,
         -1.0f, -1.0f,

         -0.5f, -0.5f,
          0.5f, -0.5f,
          0.0f,  0.5f,

         -1.0f, -0.1f,
         -1.0f, -0.2f,
         -0.25f, 0.0f,

         -0.25f, 0.0f,
          0.2f,  0.1f,
          0.21f, 0.08f,

          -0.25f, 0.0f,
          0.21f,  0.08f,
          0.22f, 0.06f,

          -0.25f, 0.0f,
          0.22f,  0.06f,
          0.23f, 0.04f,

          -0.25f, 0.0f,
          0.23f,  0.04f,
          0.24f, 0.02f,

          -0.25f, 0.0f,
          0.24f,  0.02f,
          0.25f, 0.0f,

          -0.25f, 0.0f,
          0.25f,  0.0f,
          0.26f, -0.02f,

          0.2f, 0.1f,
          1.0f,  0.0f,
          1.0f, -0.1f,
          1.0f, -0.1f,
          0.21f, 0.08f,
          0.2f, 0.1f,

          0.21f, 0.08f,
          1.0f, -0.1f,
          1.0f, -0.2f,
          0.21f, 0.08f,
          1.0f, -0.2f,
          0.22f, 0.06f,

          0.22f, 0.06f,
          1.0f, -0.2f,
          1.0f, -0.3f,
          0.23f, 0.04f,
          1.0f, -0.3f,
          0.22f, 0.06f,

          0.23f, 0.04f,
          1.0f, -0.3f,
          1.0f, -0.4f,
          0.24f, 0.02f,
          1.0f, -0.4f,
          0.23f, 0.04f,

          0.24f, 0.02f,
          1.0f, -0.4f,
          1.0f, -0.5f,
          0.25f, 0.0f,
          1.0f, -0.5f,
          0.24f, 0.02f,

          0.25f, 0.0f,
          1.0f, -0.5f,
          1.0f, -0.6f,
          0.26f,-0.02f,
          1.0f, -0.6f,
          0.25f, 0.0f,
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * 22 * sizeof(float), positions, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << source.VertexSource << std::endl;
    std::cout << source.FragmentSource << std::endl;

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    int location = glGetUniformLocation(shader, "u_Color");

    float a = 0.0f;
    float b = 1.0f;
    float c = 1.0f;
    float d = 0.0f;
    float e = 0.0f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //t³o
        glUniform4f(location, 0.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //pryzmat
        glUniform4f(location, 1.0f, 1.0f, 1.0f, 0.5f);
        glDrawArrays(GL_TRIANGLES, 6, 3);

        //œwiat³o bia³e
        glUniform4f(location, a, a, a, 1.0f);
        glDrawArrays(GL_TRIANGLES, 9, 3);

        //kolor czerwony
        glUniform4f(location, 1.0f, b, b, 1.0f);
        glDrawArrays(GL_TRIANGLES, 12, 3);

        //kolor pomarañczowy
        if (a > 2) glUniform4f(location, 1.0f, c, b, 1.0f);
        else glUniform4f(location, 1.0f, 1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 15, 3);

        //kolor ¿ó³ty
        glUniform4f(location, 1.0f, 1.0f, b, 1.0f);
        glDrawArrays(GL_TRIANGLES, 18, 3);

        //kolor zielony
        glUniform4f(location, b, 1.0f, b, 1.0f);
        glDrawArrays(GL_TRIANGLES, 21, 3);

        //kolor niebieski
        glUniform4f(location, b, b, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 24, 3);

        //kolor fioletowy
        glUniform4f(location, 1.0f, b, 1.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 27, 3);

        //przed³u¿enie czerwonego
        glUniform4f(location, d, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 30, 6);

        //przed³u¿enie pomarañczowego
        if (b < 0) glUniform4f(location, d, e, 0.0f, 1.0f);
        else glUniform4f(location, 0.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 36, 6);

        //przed³u¿enie ¿ó³tego
        glUniform4f(location, d, d, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 42, 6);

        //przed³u¿enie zielonego
        glUniform4f(location, 0.0f, d, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 48, 6);

        //przed³u¿enie niebieskiego
        glUniform4f(location, 0.0f, 0.0f, d, 1.0f);
        glDrawArrays(GL_TRIANGLES, 54, 6);

        //przed³u¿enie fioletowego
        glUniform4f(location, d, 0.0f, d, 1.0f);
        glDrawArrays(GL_TRIANGLES, 60, 6);

        if(a <= 2.0f) a += 0.01f;
        if (a >= 2.0f && b >= 0.0f) b -= 0.01f;
        if (a >= 2.0f && c > 0.5f) c -= 0.005f;
        if (b <= 0.0f && d <= 2.0f) d += 0.01f;
        if (b <= 0.0f && e < 0.5f) e += 0.005f;
        if (d >= 2.0f)
        {
            a = 0.0f;
            b = 1.0f;
            c = 1.0f;
            d = 0.0f;
            e = 0.0f;
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}