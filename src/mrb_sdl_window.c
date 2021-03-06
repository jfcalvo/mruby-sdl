#include "mrb_sdl_window.h"

#include <mrbconf.h>
#include <mruby.h>
#include <mruby/class.h>
#include <mruby/data.h>
#include <mruby/value.h>
#include <mruby/array.h>
#include <mruby/string.h>

#include "mrb_sdl_gl_context.h"

static struct RClass* mrb_sdl_window_class = NULL;

struct mrb_data_type mrb_sdl_window_get_ptr_type = { "Window", mrb_sdl_window_free };

void
mrb_sdl_window_free(mrb_state *mrb, void *ptr)
{
  mrb_free(mrb, ptr);
}

mrb_value
mrb_sdl_window_wrap(mrb_state *mrb, SDL_Window* window)
{
  struct mrb_sdl_window* tm = (struct mrb_sdl_window*)mrb_malloc(mrb, sizeof(struct mrb_sdl_window));
  tm->window = window;
  return mrb_obj_value(Data_Wrap_Struct(mrb, mrb_sdl_window_class, &mrb_sdl_window_get_ptr_type, tm));
}

struct mrb_sdl_window*
mrb_sdl_window_get_ptr(mrb_state* mrb, mrb_value value)
{
  return DATA_CHECK_GET_PTR(mrb, value, &mrb_sdl_window_get_ptr_type, struct mrb_sdl_window);
}

unsigned int
window_flag_symbol_to_flag(mrb_state* mrb, mrb_value sym)
{
  mrb_sym flag = mrb_obj_to_sym(mrb, sym);

  if (flag == mrb_intern_cstr(mrb, "fullscreen"))
    return SDL_WINDOW_FULLSCREEN;
  if (flag == mrb_intern_cstr(mrb, "fullscreen_desktop"))
    return SDL_WINDOW_FULLSCREEN_DESKTOP;
  if (flag == mrb_intern_cstr(mrb, "opengl"))
    return SDL_WINDOW_OPENGL;
  if (flag == mrb_intern_cstr(mrb, "shown"))
    return SDL_WINDOW_SHOWN;
  if (flag == mrb_intern_cstr(mrb, "hidden"))
    return SDL_WINDOW_HIDDEN;
  if (flag == mrb_intern_cstr(mrb, "borderless"))
    return SDL_WINDOW_BORDERLESS;
  if (flag == mrb_intern_cstr(mrb, "resizable"))
    return SDL_WINDOW_RESIZABLE;
  if (flag == mrb_intern_cstr(mrb, "minimized"))
    return SDL_WINDOW_MINIMIZED;
  if (flag == mrb_intern_cstr(mrb, "maximized"))
    return SDL_WINDOW_MAXIMIZED;
  if (flag == mrb_intern_cstr(mrb, "input_grabbed"))
    return SDL_WINDOW_INPUT_GRABBED;
  if (flag == mrb_intern_cstr(mrb, "input_focus"))
    return SDL_WINDOW_INPUT_FOCUS;
  if (flag == mrb_intern_cstr(mrb, "mouse_focus"))
    return SDL_WINDOW_MOUSE_FOCUS;
  if (flag == mrb_intern_cstr(mrb, "foreign"))
    return SDL_WINDOW_FOREIGN;

  return 0;
}

unsigned int
window_flags_array_to_flags(mrb_state* mrb, mrb_value array)
{
  struct RArray *a = mrb_ary_ptr(array);
  unsigned int flags = 0;
  int i;

  for(i = 0; i < a->len; i++)
  {
    flags = flags | window_flag_symbol_to_flag(mrb, a->ptr[i]);
  }

  return flags;
}

mrb_value
mrb_sdl_window_create(mrb_state *mrb, mrb_value self)
{
  mrb_value name;
  mrb_int x, y, width, height;
  mrb_value flags_array;
  unsigned int flags;
  SDL_Window* window;

  mrb_get_args(mrb, "SiiiiA", &name, &x, &y, &width, &height, &flags_array);

  flags = window_flags_array_to_flags(mrb, flags_array);

  window = SDL_CreateWindow(mrb_string_value_ptr(mrb, name), x, y, width, height, flags);
  if(!window) {
    mrb_raisef(mrb, E_RUNTIME_ERROR, "Error creating SDL window: %S", mrb_str_new_cstr(mrb, SDL_GetError()));
  }
  return mrb_sdl_window_wrap(mrb, window);
}

mrb_value
mrb_sdl_window_destroy(mrb_state *mrb, mrb_value self)
{
  struct mrb_sdl_window* window = mrb_sdl_window_get_ptr(mrb, self);
  SDL_DestroyWindow(window->window);
  
  return mrb_nil_value();
}

mrb_value
mrb_sdl_window_create_gl_context(mrb_state *mrb, mrb_value self)
{
  SDL_GLContext context;
  struct mrb_sdl_window* window = mrb_sdl_window_get_ptr(mrb, self);
  context = SDL_GL_CreateContext(window->window);
  
  return mrb_sdl_gl_context_wrap(mrb, context);
}

mrb_value
mrb_sdl_swap_gl_window(mrb_state *mrb, mrb_value self)
{
  struct mrb_sdl_window* window = mrb_sdl_window_get_ptr(mrb, self);
  SDL_GL_SwapWindow(window->window);
  
  return self;
}

mrb_value
mrb_sdl_width(mrb_state *mrb, mrb_value self)
{
  mrb_int width, height;

  struct mrb_sdl_window* window = mrb_sdl_window_get_ptr(mrb, self);
  SDL_GetWindowSize(window->window, &width, &height);

  return mrb_fixnum_value(width);
}

mrb_value
mrb_sdl_height(mrb_state *mrb, mrb_value self)
{
  mrb_int width, height;

  struct mrb_sdl_window* window = mrb_sdl_window_get_ptr(mrb, self);
  SDL_GetWindowSize(window->window, &width, &height);
  
  return mrb_fixnum_value(height);
}

void
init_mrb_sdl_window(mrb_state* mrb, struct RClass* mrb_sdl_class)
{
  mrb_sdl_window_class = mrb_define_class_under(mrb, mrb_sdl_class, "Window", mrb->object_class);
  MRB_SET_INSTANCE_TT(mrb_sdl_window_class, MRB_TT_DATA);

  mrb_define_class_method(mrb, mrb_sdl_window_class, "create", mrb_sdl_window_create, ARGS_REQ(6));
  mrb_define_method(mrb, mrb_sdl_window_class, "destroy", mrb_sdl_window_destroy, ARGS_NONE());

  mrb_define_method(mrb, mrb_sdl_window_class, "create_gl_context", mrb_sdl_window_create_gl_context, ARGS_NONE());
  mrb_define_method(mrb, mrb_sdl_window_class, "swap_gl_window", mrb_sdl_swap_gl_window, ARGS_NONE());

  mrb_define_method(mrb, mrb_sdl_window_class, "width", mrb_sdl_width, ARGS_NONE());
  mrb_define_method(mrb, mrb_sdl_window_class, "height", mrb_sdl_height, ARGS_NONE());
}