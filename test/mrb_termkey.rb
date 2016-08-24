##
## TermKey Test
##

assert("TermKey#new") do
  t = TermKey.new 
  assert_equal(TermKey, t.class)
end

assert("TermKey.waitkey") do
  t = TermKey.new
#  ret, key = t.waitkey
#  assert_equal("hello bye", t.bye)
end

assert("TermKey.get_buffer_size") do
  t = TermKey.new
  assert_equal(256, t.get_buffer_size)
end

assert("TermKey.set_buffer_size") do
  t = TermKey.new
  assert_equal(1, t.set_buffer_size(100))
  assert_equal(100, t.get_buffer_size)
end

assert("TermKey.get_buffer_remaining") do
  t = TermKey.new
  assert_equal(256, t.get_buffer_remaining)
end
