#ifndef gui_h
#define gui_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "../nuklear.h"
#include "nuklear_glfw_gl3.h"
#include "../items/base_item.h"
#include "../requestType/result_listing.h"
#include "../perf.h"
#include <Windows.h>
#include <iostream>
#include <sstream>

#define WINDOW_WIDTH 300
#define WINDOW_HEIGHT 200

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

static void error_callback(int e, const char* d)
{
	printf("Error %d: %s\n", e, d);
}

void calculate_window_position(int win_width, int win_height, int* x, int* y)
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	POINT cursor;
	GetCursorPos(&cursor);
	*x = cursor.x + (cursor.x < mode->width / 2 ? 0 : -win_width);
	*y = cursor.y + (cursor.y < mode->height / 2 ? 0 : -win_height);
}

void remove_window_borders(HWND process_window)
{
	LONG lStyle = GetWindowLong(process_window, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	SetWindowLong(process_window, GWL_STYLE, lStyle);
	LONG lExStyle = GetWindowLong(process_window, GWL_EXSTYLE);
	lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	SetWindowLong(process_window, GWL_EXSTYLE, lExStyle);
	SetWindowPos(process_window, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void run_display(items::base_item* item, poerequest::result_listing* listings)
{
	static GLFWwindow* win;
	int width = 0, height = 0;
	struct nk_context* ctx;
	struct nk_colorf bg;

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		fprintf(stdout, "[GFLW] failed to init!\n");
		exit(1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Demo", NULL, NULL);
	glfwMakeContextCurrent(win);
	HWND process_window = glfwGetWin32Window(win);
	remove_window_borders(process_window);

	int xpos, ypos;
	glfwGetWindowSize(win, &width, &height);
	calculate_window_position(width, height, &xpos, &ypos);
	glfwSetWindowPos(win, xpos, ypos);

	glViewport(0, 0, width, height);
	glewExperimental = 1;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to setup GLEW\n");
		exit(1);
	}

	ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);
	{
		struct nk_font_atlas* atlas;
		nk_glfw3_font_stash_begin(&atlas);
		nk_glfw3_font_stash_end();
	}


	bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
	glfwSetWindowFocusCallback(win, [](GLFWwindow* window, int focused)
	{
		if (!focused)
			glfwSetWindowShouldClose(window, true);
	});
	while (!glfwWindowShouldClose(win))
	{
		glfwPollEvents();
		nk_glfw3_new_frame();
		std::string full_item_name = item->name;
		if (nk_begin(ctx, full_item_name.c_str(), nk_rect(0, 0, width, height),
			NK_WINDOW_TITLE))
		{
			for (auto& row : listings->result)
			{
				nk_layout_row_dynamic(ctx, 20, 2);
				std::stringstream listing_label;
				listing_label << row.listing.price.amount << " " << row.listing.price.currency;
				nk_label(ctx, listing_label.str().c_str(), NK_TEXT_LEFT);
				nk_label(ctx, row.listing.indexed.c_str(), NK_TEXT_LEFT);

			}
		}
		nk_end(ctx);

		/* Draw */
		glfwGetWindowSize(win, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(bg.r, bg.g, bg.b, bg.a);
		nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
		glfwSwapBuffers(win);
	}
	nk_glfw3_shutdown();
	glfwTerminate();
}

#endif