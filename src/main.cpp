#include <Desktop.hpp>
#include <mruby.h>
#include <mruby/compile.h>
#include <mruby/string.h>
#include <iostream>

mrb_value mruby_test_method(mrb_state *mrb, mrb_value self)
{
	std::cout << "Hello from C++" << std::endl;
	return mrb_nil_value();
}

Desktop desktop(192, 128, 4);

int main()
{
	mrb_state *mrb = mrb_open();
	mrb_define_method(mrb, mrb->object_class, "test_method", &mruby_test_method, MRB_ARGS_NONE());
	mrb_load_string(mrb, "$test = 'This is a string'");
	mrb_load_string(mrb, "def from_ruby\nputs 'Hello from Ruby'\nend");
	mrb_load_string(mrb, "puts $test");
	mrb_load_string(mrb, "test_method");
	if (mrb_obj_respond_to(mrb, mrb->object_class, mrb_intern_cstr(mrb, "from_ruby")))
		mrb_funcall(mrb, mrb_nil_value(), "from_ruby", 0);
	else
		std::cout << "Seems that 'from_ruby' doesn't exists" << std::endl;
	mrb_close(mrb);
	desktop.run();
	return 0;
}