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
  TermKey *tk = termkey_new(0, 0);
  char *str;
  int len;

  DATA_TYPE(self) = &mrb_termkey_data_type;
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
     printf("ret = %d\n", ret);
     mrb_ary_push(mrb, ret_ary, mrb_fixnum_value(ret));
     DATA_TYPE(key_obj) = &mrb_termkeykey_data_type;
     DATA_PTR(key_obj) = key;
     mrb_ary_push(mrb, ret_ary, key_obj);
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

static mrb_value mrb_termkeykey_codepoint(mrb_state *mrb, mrb_value self)
{
     TermKeyKey *tk = DATA_PTR(self);
     return mrb_fixnum_value(tk->code.codepoint);
}

void mrb_mruby_termkey_gem_init(mrb_state *mrb)
{
     struct RClass *termkey, *termkeykey;
     termkey = mrb_define_class(mrb, "TermKey", mrb->object_class);
     MRB_SET_INSTANCE_TT(termkey, MRB_TT_DATA);
     termkeykey = mrb_define_class_under(mrb, termkey, "Key", mrb->object_class);
     MRB_SET_INSTANCE_TT(termkeykey, MRB_TT_DATA);
     
    mrb_define_method(mrb, termkey, "initialize", mrb_termkey_init, MRB_ARGS_NONE());
    mrb_define_method(mrb, termkey, "waitkey", mrb_termkey_waitkey, MRB_ARGS_NONE());

    mrb_define_method(mrb, termkeykey, "type", mrb_termkeykey_type, MRB_ARGS_NONE());
    mrb_define_method(mrb, termkeykey, "modifiers", mrb_termkeykey_modifiers, MRB_ARGS_NONE());
    mrb_define_method(mrb, termkeykey, "codepoint", mrb_termkeykey_codepoint, MRB_ARGS_NONE());

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
    DONE;
}

void mrb_mruby_termkey_gem_final(mrb_state *mrb)
{
}

