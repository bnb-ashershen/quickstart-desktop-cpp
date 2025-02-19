#include "GlfwWindow.hpp"

#include <bnb/effect_player/utility.hpp>
#include <bnb/utils/defs.hpp>
#if BNB_OS_WINDOWS
#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif // !GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif



#include <map>
#include <utility>

#ifndef GL_TEXTURE_CUBE_MAP_SEAMLESS
    #define GL_TEXTURE_CUBE_MAP_SEAMLESS 0x884F
#endif

using namespace bnb;

GlfwWindow::GlfwWindow(const std::string& title)
{
    init();
    try {
        create_window(title);

#if BNB_GL_BACKEND
        glfwMakeContextCurrent(m_window);

        load_glad_functions();
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); 

        glfwMakeContextCurrent(nullptr);
#endif
    } catch (...) {
        glfwTerminate();
        throw;
    }
}

GlfwWindow::~GlfwWindow()
{

#if BNB_GL_BACKEND
    glfwMakeContextCurrent(nullptr);
#endif

    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void GlfwWindow::show(uint32_t width_hint, uint32_t height_hint)
{
    async::spawn(
        m_scheduler,
        [this, width_hint, height_hint]() {
            glfwSetWindowSize(m_window, width_hint, height_hint);
            glfwSetWindowPos(m_window, 0, 0);
            glfwShowWindow(m_window);
        });

    glfwPostEmptyEvent();
}

void GlfwWindow::run_main_loop()
{
    while (!glfwWindowShouldClose(m_window)) {
        glfwWaitEvents();
        m_scheduler.run_all_tasks();
    }
}

void GlfwWindow::init()
{
    if (GLFW_TRUE != glfwInit()) {
        throw std::runtime_error("glfwInit error");
    }
}

void GlfwWindow::create_window(const std::string& title)
{
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#if BNB_GL_BACKEND
    glfwWindowHint(GLFW_DEPTH_BITS, 0);
    glfwWindowHint(GLFW_STENCIL_BITS, 0);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);

    #if BNB_OS_WINDOWS
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    #elif BNB_OS_MACOS || BNB_OS_LINUX
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    #endif
#else
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif // BNB_GL_BACKEND

    //
    // Create window
    //

    // The new window is hidden until is shown, so
    // initial width and height are just dummy values
    auto initial_window_width = 1;
    auto initial_window_height = 1;

    m_window = glfwCreateWindow(
        initial_window_width,
        initial_window_height,
        title.c_str(),
        nullptr,
        nullptr);

    if (nullptr == m_window) {
        throw std::runtime_error("glfwCreateWindow error");
    }
}

void GlfwWindow::load_glad_functions()
{
#if BNB_OS_WINDOWS || BNB_OS_MACOS
    // it's only need for use while working with dynamic libs
    utility::load_gl_functions();
#endif

}

#if BNB_OS_WINDOWS
void* GlfwWindow::get_surface() const
{
    return glfwGetWin32Window(m_window);
}
#endif
