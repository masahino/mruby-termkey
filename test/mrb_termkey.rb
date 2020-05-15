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

assert("TermKey.buffer_size") do
  t = TermKey.new
  assert_equal(256, t.buffer_size)
end

assert("TermKey.buffer_size=") do
  t = TermKey.new
  t.buffer_size = 100
#  assert_equal(1, t.buffer_size=100)
  assert_equal(100, t.buffer_size)
end

assert("TermKey.buffer_remaining") do
  t = TermKey.new
  assert_equal(256, t.buffer_remaining)
end

assert("TermKey.waittime") do
  t = TermKey.new
  assert_equal(50, t.waittime)
  t.waittime = 100
  assert_equal(100, t.waittime)
end

assert("TermKey.strpkey") do
  t = TermKey.new
  key = t.strpkey("Up", 0)
  assert_equal(TermKey::TYPE_KEYSYM, key.type)
  assert_equal(TermKey::SYM_UP, key.code)
end
