#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include "requestType/overview.h"
#include "requestType/result_listing.h"
#include "request.h"
#include "item_parser.h"

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <limits.h>

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
#define NK_GLFW_GL4_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define NK_IMPLEMENTATION
#include "nuklear.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <Windows.h>
#include <thread>
#include "hotkeys/hotkeys.h"
using json = nlohmann::json;
HWND Proccess_window;
HWND Console_window;

static void error_callback(int e, const char* d)
{
	printf("Error %d: %s\n", e, d);
}

static void text_input(GLFWwindow* win, unsigned int codepoint)
{
	nk_input_unicode((struct nk_context*)glfwGetWindowUserPointer(win), codepoint);
}
static void scroll_input(GLFWwindow* win, double _, double yoff)
{
	nk_input_scroll((struct nk_context*)glfwGetWindowUserPointer(win), nk_vec2(0, (float)yoff));
}

struct media {
	GLint skin;
	struct nk_image menu;
	struct nk_image check;
	struct nk_image check_cursor;
	struct nk_image option;
	struct nk_image option_cursor;
	struct nk_image header;
	struct nk_image window;
	struct nk_image scrollbar_inc_button;
	struct nk_image scrollbar_inc_button_hover;
	struct nk_image scrollbar_dec_button;
	struct nk_image scrollbar_dec_button_hover;
	struct nk_image button;
	struct nk_image button_hover;
	struct nk_image button_active;
	struct nk_image tab_minimize;
	struct nk_image tab_maximize;
	struct nk_image slider;
	struct nk_image slider_hover;
	struct nk_image slider_active;
};


/* ===============================================================
 *
 *                          DEVICE
 *
 * ===============================================================*/
struct nk_glfw_vertex {
	float position[2];
	float uv[2];
	nk_byte col[4];
};

struct device {
	struct nk_buffer cmds;
	struct nk_draw_null_texture null;
	GLuint vbo, vao, ebo;
	GLuint prog;
	GLuint vert_shdr;
	GLuint frag_shdr;
	GLint attrib_pos;
	GLint attrib_uv;
	GLint attrib_col;
	GLint uniform_tex;
	GLint uniform_proj;
	GLuint font_tex;
};
#define NK_SHADER_VERSION "#version 300 es\n"
static void
device_init(struct device* dev)
{
	GLint status;
	static const GLchar* vertex_shader =
		NK_SHADER_VERSION
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 TexCoord;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main() {\n"
		"   Frag_UV = TexCoord;\n"
		"   Frag_Color = Color;\n"
		"   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
		"}\n";
	static const GLchar* fragment_shader =
		NK_SHADER_VERSION
		"precision mediump float;\n"
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main(){\n"
		"   Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
		"}\n";

	nk_buffer_init_default(&dev->cmds);
	dev->prog = glCreateProgram();
	dev->vert_shdr = glCreateShader(GL_VERTEX_SHADER);
	dev->frag_shdr = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(dev->vert_shdr, 1, &vertex_shader, 0);
	glShaderSource(dev->frag_shdr, 1, &fragment_shader, 0);
	glCompileShader(dev->vert_shdr);
	glCompileShader(dev->frag_shdr);
	glGetShaderiv(dev->vert_shdr, GL_COMPILE_STATUS, &status);
	assert(status == GL_TRUE);
	glGetShaderiv(dev->frag_shdr, GL_COMPILE_STATUS, &status);
	assert(status == GL_TRUE);
	glAttachShader(dev->prog, dev->vert_shdr);
	glAttachShader(dev->prog, dev->frag_shdr);
	glLinkProgram(dev->prog);
	glGetProgramiv(dev->prog, GL_LINK_STATUS, &status);
	assert(status == GL_TRUE);

	dev->uniform_tex = glGetUniformLocation(dev->prog, "Texture");
	dev->uniform_proj = glGetUniformLocation(dev->prog, "ProjMtx");
	dev->attrib_pos = glGetAttribLocation(dev->prog, "Position");
	dev->attrib_uv = glGetAttribLocation(dev->prog, "TexCoord");
	dev->attrib_col = glGetAttribLocation(dev->prog, "Color");

	{
		/* buffer setup */
		GLsizei vs = sizeof(struct nk_glfw_vertex);
		size_t vp = offsetof(struct nk_glfw_vertex, position);
		size_t vt = offsetof(struct nk_glfw_vertex, uv);
		size_t vc = offsetof(struct nk_glfw_vertex, col);

		glGenBuffers(1, &dev->vbo);
		glGenBuffers(1, &dev->ebo);
		glGenVertexArrays(1, &dev->vao);

		glBindVertexArray(dev->vao);
		glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

		glEnableVertexAttribArray((GLuint)dev->attrib_pos);
		glEnableVertexAttribArray((GLuint)dev->attrib_uv);
		glEnableVertexAttribArray((GLuint)dev->attrib_col);

		glVertexAttribPointer((GLuint)dev->attrib_pos, 2, GL_FLOAT, GL_FALSE, vs, (void*)vp);
		glVertexAttribPointer((GLuint)dev->attrib_uv, 2, GL_FLOAT, GL_FALSE, vs, (void*)vt);
		glVertexAttribPointer((GLuint)dev->attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, vs, (void*)vc);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

static void
device_draw(struct device* dev, struct nk_context* ctx, int width, int height,
	struct nk_vec2 scale, enum nk_anti_aliasing AA)
{
	GLfloat ortho[4][4] = {
		{2.0f, 0.0f, 0.0f, 0.0f},
		{0.0f,-2.0f, 0.0f, 0.0f},
		{0.0f, 0.0f,-1.0f, 0.0f},
		{-1.0f,1.0f, 0.0f, 1.0f},
	};
	ortho[0][0] /= (GLfloat)width;
	ortho[1][1] /= (GLfloat)height;

	/* setup global state */
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	/* setup program */
	glUseProgram(dev->prog);
	glUniform1i(dev->uniform_tex, 0);
	glUniformMatrix4fv(dev->uniform_proj, 1, GL_FALSE, &ortho[0][0]);
	{
		/* convert from command queue into draw list and draw to screen */
		const struct nk_draw_command* cmd;
		void* vertices, * elements;
		const nk_draw_index* offset = NULL;

		/* allocate vertex and element buffer */
		glBindVertexArray(dev->vao);
		glBindBuffer(GL_ARRAY_BUFFER, dev->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dev->ebo);

		glBufferData(GL_ARRAY_BUFFER, MAX_VERTEX_MEMORY, NULL, GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_ELEMENT_MEMORY, NULL, GL_STREAM_DRAW);

		/* load draw vertices & elements directly into vertex + element buffer */
		vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		elements = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
		{
			/* fill convert configuration */
			struct nk_convert_config config;
			static const struct nk_draw_vertex_layout_element vertex_layout[] = {
				{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_glfw_vertex, position)},
				{NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct nk_glfw_vertex, uv)},
				{NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(struct nk_glfw_vertex, col)},
				{NK_VERTEX_LAYOUT_END}
			};
			NK_MEMSET(&config, 0, sizeof(config));
			config.vertex_layout = vertex_layout;
			config.vertex_size = sizeof(struct nk_glfw_vertex);
			config.vertex_alignment = NK_ALIGNOF(struct nk_glfw_vertex);
			config.null = dev->null;
			config.circle_segment_count = 22;
			config.curve_segment_count = 22;
			config.arc_segment_count = 22;
			config.global_alpha = 1.0f;
			config.shape_AA = AA;
			config.line_AA = AA;

			/* setup buffers to load vertices and elements */
			{struct nk_buffer vbuf, ebuf;
			nk_buffer_init_fixed(&vbuf, vertices, MAX_VERTEX_MEMORY);
			nk_buffer_init_fixed(&ebuf, elements, MAX_ELEMENT_MEMORY);
			nk_convert(ctx, &dev->cmds, &vbuf, &ebuf, &config); }
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

		/* iterate over and execute each draw command */
		nk_draw_foreach(cmd, ctx, &dev->cmds)
		{
			if (!cmd->elem_count) continue;
			glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id);
			glScissor(
				(GLint)(cmd->clip_rect.x * scale.x),
				(GLint)((height - (GLint)(cmd->clip_rect.y + cmd->clip_rect.h)) * scale.y),
				(GLint)(cmd->clip_rect.w * scale.x),
				(GLint)(cmd->clip_rect.h * scale.y));
			glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT, offset);
			offset += cmd->elem_count;
		}
		nk_clear(ctx);
	}

	/* default OpenGL state */
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
	glDisable(GL_SCISSOR_TEST);
}

static void
device_shutdown(struct device* dev)
{
	glDetachShader(dev->prog, dev->vert_shdr);
	glDetachShader(dev->prog, dev->frag_shdr);
	glDeleteShader(dev->vert_shdr);
	glDeleteShader(dev->frag_shdr);
	glDeleteProgram(dev->prog);
	glDeleteTextures(1, &dev->font_tex);
	glDeleteBuffers(1, &dev->vbo);
	glDeleteBuffers(1, &dev->ebo);
	nk_buffer_free(&dev->cmds);
}

static void
device_upload_atlas(struct device* dev, const void* image, int width, int height)
{
	glGenTextures(1, &dev->font_tex);
	glBindTexture(GL_TEXTURE_2D, dev->font_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image);
}

void remove_window_borders()
{
	LONG lStyle = GetWindowLong(Proccess_window, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	SetWindowLong(Proccess_window, GWL_STYLE, lStyle);
	LONG lExStyle = GetWindowLong(Proccess_window, GWL_EXSTYLE);
	lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	SetWindowLong(Proccess_window, GWL_EXSTYLE, lExStyle);
	SetWindowPos(Proccess_window, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

void glfw_setup()
{
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		fprintf(stdout, "[GFLW] failed to init!\n");
		exit(1);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

#define LEN(a) (sizeof(a)/sizeof(a)[0])


GLFWwindow* price_check_window;
void window_focus_callback(GLFWwindow* window, int focused)
{
	if (!focused)
		glfwSetWindowShouldClose(window, true);
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

void run_display(items::base_item* item, poerequest::result_listing* listings)
{
	/* Platform */
	int width = 300;
	int height = 200;
	int display_width = 0;
	int display_height = 0;

	/* GUI */
	struct device device;
	struct nk_font_atlas atlas;
	struct media media;
	struct nk_context ctx;
	struct nk_font* font;
	glfw_setup();
	price_check_window = glfwCreateWindow(width, height, "Demo", NULL, NULL);

	glfwMakeContextCurrent(price_check_window);
	glfwSetWindowUserPointer(price_check_window, &ctx);
	glfwSetCharCallback(price_check_window, text_input);
	glfwSetScrollCallback(price_check_window, scroll_input);

	int xpos;
	int ypos;
	calculate_window_position(width, height, &xpos, &ypos);
	glfwSetWindowPos(price_check_window, xpos, ypos);


	glfwGetFramebufferSize(price_check_window, &display_width, &display_height);

	/* OpenGL */
	glViewport(0, 0, width, height);
	glewExperimental = 1;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to setup GLEW\n");
		exit(1);
	}

	{
		device_init(&device);

		{
			const void* image; int w, h;
			nk_font_atlas_init_default(&atlas);
			nk_font_atlas_begin(&atlas);

			font = nk_font_atlas_add_default(&atlas, 13.0f, NULL);
			image = nk_font_atlas_bake(&atlas, &w, &h, NK_FONT_ATLAS_RGBA32);
			device_upload_atlas(&device, image, w, h);
			nk_font_atlas_end(&atlas, nk_handle_id((int)device.font_tex), &device.null);
		}
		nk_init_default(&ctx, &font->handle);
	}
	Proccess_window = glfwGetWin32Window(price_check_window);

	remove_window_borders();

	glfwSetWindowFocusCallback(price_check_window, window_focus_callback);
	while (!glfwWindowShouldClose(price_check_window))
	{
		/* High DPI displays */
		struct nk_vec2 scale;
		glfwGetWindowSize(price_check_window, &width, &height);
		glfwGetFramebufferSize(price_check_window, &display_width, &display_height);
		scale.x = (float)display_width / (float)width;
		scale.y = (float)display_height / (float)height;

		/* Input */
		{
			double x, y;
			nk_input_begin(&ctx);
			glfwPollEvents();
			nk_input_key(&ctx, NK_KEY_DEL, glfwGetKey(price_check_window, GLFW_KEY_DELETE) == GLFW_PRESS);
			nk_input_key(&ctx, NK_KEY_ENTER, glfwGetKey(price_check_window, GLFW_KEY_ENTER) == GLFW_PRESS);
			nk_input_key(&ctx, NK_KEY_TAB, glfwGetKey(price_check_window, GLFW_KEY_TAB) == GLFW_PRESS);
			nk_input_key(&ctx, NK_KEY_BACKSPACE, glfwGetKey(price_check_window, GLFW_KEY_BACKSPACE) == GLFW_PRESS);
			nk_input_key(&ctx, NK_KEY_LEFT, glfwGetKey(price_check_window, GLFW_KEY_LEFT) == GLFW_PRESS);
			nk_input_key(&ctx, NK_KEY_RIGHT, glfwGetKey(price_check_window, GLFW_KEY_RIGHT) == GLFW_PRESS);
			nk_input_key(&ctx, NK_KEY_UP, glfwGetKey(price_check_window, GLFW_KEY_UP) == GLFW_PRESS);
			nk_input_key(&ctx, NK_KEY_DOWN, glfwGetKey(price_check_window, GLFW_KEY_DOWN) == GLFW_PRESS);
			if (glfwGetKey(price_check_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
				glfwGetKey(price_check_window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) {
				nk_input_key(&ctx, NK_KEY_COPY, glfwGetKey(price_check_window, GLFW_KEY_C) == GLFW_PRESS);
				nk_input_key(&ctx, NK_KEY_PASTE, glfwGetKey(price_check_window, GLFW_KEY_P) == GLFW_PRESS);
				nk_input_key(&ctx, NK_KEY_CUT, glfwGetKey(price_check_window, GLFW_KEY_X) == GLFW_PRESS);
				nk_input_key(&ctx, NK_KEY_CUT, glfwGetKey(price_check_window, GLFW_KEY_E) == GLFW_PRESS);
				nk_input_key(&ctx, NK_KEY_SHIFT, 1);
			}
			else {
				nk_input_key(&ctx, NK_KEY_COPY, 0);
				nk_input_key(&ctx, NK_KEY_PASTE, 0);
				nk_input_key(&ctx, NK_KEY_CUT, 0);
				nk_input_key(&ctx, NK_KEY_SHIFT, 0);
			}
			glfwGetCursorPos(price_check_window, &x, &y);
			nk_input_motion(&ctx, (int)x, (int)y);
			nk_input_button(&ctx, NK_BUTTON_LEFT, (int)x, (int)y, glfwGetMouseButton(price_check_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
			nk_input_button(&ctx, NK_BUTTON_MIDDLE, (int)x, (int)y, glfwGetMouseButton(price_check_window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);
			nk_input_button(&ctx, NK_BUTTON_RIGHT, (int)x, (int)y, glfwGetMouseButton(price_check_window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
			nk_input_end(&ctx);
		}

		/* GUI */
		{
			struct nk_panel layout, tab;
			std::string full_item_name = item->name;
			if (nk_begin(&ctx, full_item_name.c_str(), nk_rect(0, 0, width, height),
				NK_WINDOW_TITLE))
			{
				for (auto& row : listings->result)
				{
					nk_layout_row_dynamic(&ctx, 20, 2);
					std::stringstream listing_label;
					listing_label << row.listing.price.amount << " " << row.listing.price.currency;
					nk_label(&ctx, listing_label.str().c_str(), NK_TEXT_LEFT);
					nk_label(&ctx, row.listing.indexed.c_str(), NK_TEXT_LEFT);

				}
			}
			nk_end(&ctx);
		}

		/* Draw */
		glViewport(0, 0, display_width, display_height);
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.5882, 0.6666, 0.6666, 1.0f);
		device_draw(&device, &ctx, width, height, scale, NK_ANTI_ALIASING_ON);
		glfwSwapBuffers(price_check_window);
	}
	glDeleteTextures(1, (const GLuint*)& media.skin);
	nk_font_atlas_clear(&atlas);
	nk_free(&ctx);
	device_shutdown(&device);
	glfwTerminate();
}

int main()
{
	Console_window = GetConsoleWindow();
	hotkeys::register_hotkeys();

	curl_global_init(CURL_GLOBAL_ALL);
	std::cout << "Poe Fast Pricer" << std::endl;

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (msg.message == WM_HOTKEY)
		{
			hotkeys::run_price_check([](items::base_item* item, poerequest::result_listing* listings) {
				run_display(item, listings);
			});
		}
	}

	curl_global_cleanup();
	return 0;
}