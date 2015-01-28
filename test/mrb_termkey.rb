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

