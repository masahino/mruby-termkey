/*
** mrb_termkey.c - TermKey class
**
** Copyright (c) mruby-termkey developers 2015
**
** See Copyright Notice in LICENSE
*/

#include <termkey.h>

#include "mruby.h"
#include "mruby/class.h"
#include "mruby/data.h"
#include "mruby/array.h"
#include "mrb_termkey.h"

#define DONE mrb_gc_arena_restore(mrb, 0);

static void mrb_termkey_free(mrb_state *mrb, void *ptr) {
     if (ptr != NULL) {
	  termkey_destroy(ptr);
     }
}

static const struct mrb_data_type mrb_termkey_data_type = {
  "mrb_termkey_data", mrb_termkey_free,
};

static const struct mrb_data_type mrb_termkeykey_data_type = {
  "mrb_termkeykey_data", mrb_free,
};

static mrb_value mrb_termkey_init(mrb_state *mrb, mrb_value self)
{
  TermKey *tk = NULL;
  mrb_int argc, fd, flags;

  DATA_TYPE(self) = &mrb_termkey_data_type;
  DATA_PTR(self) = NULL;

  argc = mrb_get_args(mrb, "|ii", &fd, &flags);
  if (argc == 0) {
    fd = 0;
  }
  if (argc < 2) {
    flags = 0;
  }
  tk = termkey_new(fd, flags);
  DATA_PTR(self) = tk;

  return self;
}

static mrb_value mrb_termkey_waitkey(mrb_state *mrb, mrb_value self)
{
     TermKey *tk = DATA_PTR(self);
     TermKeyResult ret;
     TermKeyKey *key;
     struct RClass *termkeykey = mrb_class_get_under(mrb, mrb_obj_class(mrb, self), "Key");
     mrb_value ret_ary = mrb_ary_new(mrb);
     mrb_value key_obj = mrb_obj_value(Data_Wrap_Struct(mrb, termkeykey, &mrb_termkeykey_data_type, NULL));

     key = mrb_malloc(mrb, sizeof(TermKeyKey));
     ret = termkey_waitkey(tk, key);
     mrb_ary_push(mrb, ret_ary, mrb_fixnum_value(ret));
     DATA_TYPE(key_obj) = &mrb_termkeykey_data_type;
     DATA_PTR(key_obj) = key;
     mrb_ary_push(mrb, ret_ary, key_obj);
     return ret_ary;
}

static mrb_value mrb_termkey_strfkey(mrb_state *mrb, mrb_value self)
{
     TermKey *tk = DATA_PTR(self);
     TermKeyKey *key;
     char buff[64];
     mrb_int format, ret;
     mrb_value key_obj;

     mrb_get_args(mrb, "oi", &key_obj, &format);
     key = DATA_PTR(key_obj);
     ret = termkey_strfkey(tk, buff, sizeof(buff), key, format);
     return mrb_str_new_cstr(mrb, buff);
}

static mrb_value mrb_termkey_interpret_mouse(mrb_state *mrb, mrb_value self)
{
     TermKey *tk = DATA_PTR(self);
     TermKeyKey *key;
     TermKeyMouseEvent ev;
     int button, line, col;
     mrb_value key_obj;
     mrb_value ret_ary = mrb_ary_new(mrb);

     mrb_get_args(mrb, "o", &key_obj);
     key = DATA_PTR(key_obj);
     termkey_interpret_mouse(tk, key, &ev, &button, &line, &col);
     mrb_ary_push(mrb, ret_ary, mrb_fixnum_value(ev));
     mrb_ary_push(mrb, ret_ary, mrb_fixnum_value(button));
     mrb_ary_push(mrb, ret_ary, mrb_fixnum_value(line));
     mrb_ary_push(mrb, ret_ary, mrb_fixnum_value(col));
     return ret_ary;
}

static mrb_value mrb_termkeykey_type(mrb_state *mrb, mrb_value self)
{
     TermKeyKey *tk = DATA_PTR(self);
     return mrb_fixnum_value(tk->type);
}

static mrb_value mrb_termkeykey_modifiers(mrb_state *mrb, mrb_value self)
{
     TermKeyKey *tk = DATA_PTR(self);
     return mrb_fixnum_value(tk->modifiers);
}

static mrb_value mrb_termkeykey_code(mrb_state *mrb, mrb_value self)
{
     TermKeyKey *tk = DATA_PTR(self);
     mrb_value code;

     switch(tk->type) {
     case TERMKEY_TYPE_UNICODE:
	  code = mrb_fixnum_value(tk->code.codepoint);
	  break;
     case TERMKEY_TYPE_FUNCTION:
	  code = mrb_fixnum_value(tk->code.number);
	  break;
     case TERMKEY_TYPE_KEYSYM:
	  code = mrb_fixnum_value(tk->code.sym);
	  break;
     case  TERMKEY_TYPE_MOUSE:
	  code = mrb_str_new(mrb, tk->code.mouse, 4);
	  break;
     case TERMKEY_TYPE_POSITION:
     case TERMKEY_TYPE_MODEREPORT:
     case TERMKEY_TYPE_UNKNOWN_CSI:
     default:
	  code = mrb_nil_value();
     }
     return code;
}

static mrb_value mrb_termkeykey_utf8(mrb_state *mrb, mrb_value self)
{
     TermKeyKey *tk = DATA_PTR(self);
     return mrb_str_new_cstr(mrb, tk->utf8);
}


void mrb_mruby_termkey_gem_init(mrb_state *mrb)
{
     struct RClass *termkey, *termkeykey;
     termkey = mrb_define_class(mrb, "TermKey", mrb->object_class);
     MRB_SET_INSTANCE_TT(termkey, MRB_TT_DATA);
     termkeykey = mrb_define_class_under(mrb, termkey, "Key", mrb->object_class);
     MRB_SET_INSTANCE_TT(termkeykey, MRB_TT_DATA);
     
    mrb_define_method(mrb, termkey, "initialize", mrb_termkey_init, MRB_ARGS_OPT(2));
    mrb_define_method(mrb, termkey, "waitkey", mrb_termkey_waitkey, MRB_ARGS_NONE());
    mrb_define_method(mrb, termkey, "strfkey", mrb_termkey_strfkey, MRB_ARGS_REQ(2));
    mrb_define_method(mrb, termkey, "interpret_mouse", mrb_termkey_interpret_mouse, MRB_ARGS_REQ(2));

    mrb_define_method(mrb, termkeykey, "type", mrb_termkeykey_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, termkeykey, "modifiers", mrb_termkeykey_modifiers, MRB_ARGS_NONE());
    mrb_define_method(mrb, termkeykey, "code", mrb_termkeykey_code, MRB_ARGS_NONE());
    mrb_define_method(mrb, termkeykey, "utf8", mrb_termkeykey_utf8, MRB_ARGS_NONE());

    mrb_define_const(mrb, termkey, "TYPE_UNICODE", mrb_fixnum_value(TERMKEY_TYPE_UNICODE));
    mrb_define_const(mrb, termkey, "TYPE_FUNCTION", mrb_fixnum_value(TERMKEY_TYPE_FUNCTION));
    mrb_define_const(mrb, termkey, "TYPE_KEYSYM", mrb_fixnum_value(TERMKEY_TYPE_KEYSYM));
    mrb_define_const(mrb, termkey, "TYPE_MOUSE", mrb_fixnum_value(TERMKEY_TYPE_MOUSE));
    mrb_define_const(mrb, termkey, "TYPE_POSITION", mrb_fixnum_value(TERMKEY_TYPE_POSITION));
    mrb_define_const(mrb, termkey, "TYPE_MODEREPORT", mrb_fixnum_value(TERMKEY_TYPE_MODEREPORT));
    mrb_define_const(mrb, termkey, "TYPE_UNKNOWN_CSI", mrb_fixnum_value(TERMKEY_TYPE_UNKNOWN_CSI));
    mrb_define_const(mrb, termkey, "RES_NONE", mrb_fixnum_value(TERMKEY_RES_NONE));
    mrb_define_const(mrb, termkey, "RES_KEY", mrb_fixnum_value(TERMKEY_RES_KEY));
    mrb_define_const(mrb, termkey, "RES_EOF", mrb_fixnum_value(TERMKEY_RES_EOF));
    mrb_define_const(mrb, termkey, "RES_AGAIN", mrb_fixnum_value(TERMKEY_RES_AGAIN));
    mrb_define_const(mrb, termkey, "RES_ERROR", mrb_fixnum_value(TERMKEY_RES_ERROR));

    mrb_define_const(mrb, termkey, "KEYMOD_SHIFT", mrb_fixnum_value(TERMKEY_KEYMOD_SHIFT));
    mrb_define_const(mrb, termkey, "KEYMOD_ALT", mrb_fixnum_value(TERMKEY_KEYMOD_ALT));
    mrb_define_const(mrb, termkey, "KEYMOD_CTRL", mrb_fixnum_value(TERMKEY_KEYMOD_CTRL));

    /* TermKeySym */
    mrb_define_const(mrb, termkey, "SYM_UNKNOWN", mrb_fixnum_value(TERMKEY_SYM_UNKNOWN));
    mrb_define_const(mrb, termkey, "SYM_NONE", mrb_fixnum_value(TERMKEY_SYM_NONE));
    /* Special names in C0 */
    mrb_define_const(mrb, termkey, "SYM_BACKSPACE", mrb_fixnum_value(TERMKEY_SYM_BACKSPACE));
    mrb_define_const(mrb, termkey, "SYM_TAB", mrb_fixnum_value(TERMKEY_SYM_TAB));
    mrb_define_const(mrb, termkey, "SYM_ENTER", mrb_fixnum_value(TERMKEY_SYM_ENTER));
    mrb_define_const(mrb, termkey, "SYM_ESCAPE", mrb_fixnum_value(TERMKEY_SYM_ESCAPE));
    /* Special names in G0 */
    mrb_define_const(mrb, termkey, "SYM_SPACE", mrb_fixnum_value(TERMKEY_SYM_SPACE));
    mrb_define_const(mrb, termkey, "SYM_DEL", mrb_fixnum_value(TERMKEY_SYM_DEL));
    /* Special keys */
    mrb_define_const(mrb, termkey, "SYM_UP", mrb_fixnum_value(TERMKEY_SYM_UP));
    mrb_define_const(mrb, termkey, "SYM_DOWN", mrb_fixnum_value(TERMKEY_SYM_DOWN));
    mrb_define_const(mrb, termkey, "SYM_LEFT", mrb_fixnum_value(TERMKEY_SYM_LEFT));
    mrb_define_const(mrb, termkey, "SYM_RIGHT", mrb_fixnum_value(TERMKEY_SYM_RIGHT));
    mrb_define_const(mrb, termkey, "SYM_BEGIN", mrb_fixnum_value(TERMKEY_SYM_BEGIN));
    mrb_define_const(mrb, termkey, "SYM_FIND", mrb_fixnum_value(TERMKEY_SYM_FIND));
    mrb_define_const(mrb, termkey, "SYM_INSERT", mrb_fixnum_value(TERMKEY_SYM_INSERT));
    mrb_define_const(mrb, termkey, "SYM_DELETE", mrb_fixnum_value(TERMKEY_SYM_DELETE));
    mrb_define_const(mrb, termkey, "SYM_SELECT", mrb_fixnum_value(TERMKEY_SYM_SELECT));
    mrb_define_const(mrb, termkey, "SYM_PAGEUP", mrb_fixnum_value(TERMKEY_SYM_PAGEUP));
    mrb_define_const(mrb, termkey, "SYM_PAGEDOWN", mrb_fixnum_value(TERMKEY_SYM_PAGEDOWN));
    mrb_define_const(mrb, termkey, "SYM_HOME", mrb_fixnum_value(TERMKEY_SYM_HOME));
    mrb_define_const(mrb, termkey, "SYM_END", mrb_fixnum_value(TERMKEY_SYM_END));

    mrb_define_const(mrb, termkey, "FORMAT_LONGMOD", mrb_fixnum_value(TERMKEY_FORMAT_LONGMOD));
    mrb_define_const(mrb, termkey, "FORMAT_CARETCTRL",  mrb_fixnum_value(TERMKEY_FORMAT_CARETCTRL));
    mrb_define_const(mrb, termkey, "FORMAT_ALTISMETA",  mrb_fixnum_value(TERMKEY_FORMAT_ALTISMETA));
    mrb_define_const(mrb, termkey, "FORMAT_WRAPBRACKET",  mrb_fixnum_value(TERMKEY_FORMAT_WRAPBRACKET));
    mrb_define_const(mrb, termkey, "FORMAT_SPACEMOD",  mrb_fixnum_value(TERMKEY_FORMAT_SPACEMOD));
    mrb_define_const(mrb, termkey, "FORMAT_LOWERMOD",  mrb_fixnum_value(TERMKEY_FORMAT_LOWERMOD));
    mrb_define_const(mrb, termkey, "FORMAT_LOWERSPACE",  mrb_fixnum_value(TERMKEY_FORMAT_LOWERSPACE));
    mrb_define_const(mrb, termkey, "FORMAT_MOUSE_POS",  mrb_fixnum_value(TERMKEY_FORMAT_MOUSE_POS));
    mrb_define_const(mrb, termkey, "FORMAT_VIM",  mrb_fixnum_value(TERMKEY_FORMAT_VIM));
    mrb_define_const(mrb, termkey, "FORMAT_URWID",  mrb_fixnum_value(TERMKEY_FORMAT_URWID));

    mrb_define_const(mrb, termkey, "FLAG_NOINTERPRET", mrb_fixnum_value(TERMKEY_FLAG_NOINTERPRET));
    mrb_define_const(mrb, termkey, "FLAG_CONVERTKP", mrb_fixnum_value(TERMKEY_FLAG_CONVERTKP));
    mrb_define_const(mrb, termkey, "FLAG_RAW", mrb_fixnum_value(TERMKEY_FLAG_RAW));
    mrb_define_const(mrb, termkey, "FLAG_UTF8", mrb_fixnum_value(TERMKEY_FLAG_UTF8));
    mrb_define_const(mrb, termkey, "FLAG_NOTERMIOS", mrb_fixnum_value(TERMKEY_FLAG_NOTERMIOS));
    mrb_define_const(mrb, termkey, "FLAG_SPACESYMBOL", mrb_fixnum_value(TERMKEY_FLAG_SPACESYMBOL));
    mrb_define_const(mrb, termkey, "FLAG_CTRLC", mrb_fixnum_value(TERMKEY_FLAG_CTRLC));
    mrb_define_const(mrb, termkey, "FLAG_EINTR", mrb_fixnum_value(TERMKEY_FLAG_EINTR));

    mrb_define_const(mrb, termkey, "MOUSE_UNKNOWN", mrb_fixnum_value(TERMKEY_MOUSE_UNKNOWN));
    mrb_define_const(mrb, termkey, "MOUSE_PRESS", mrb_fixnum_value(TERMKEY_MOUSE_PRESS));
    mrb_define_const(mrb, termkey, "MOUSE_DRAG", mrb_fixnum_value(TERMKEY_MOUSE_DRAG));
    mrb_define_const(mrb, termkey, "MOUSE_RELEASE", mrb_fixnum_value(TERMKEY_MOUSE_RELEASE));
}

void mrb_mruby_termkey_gem_final(mrb_state *mrb)
{
}

