tk = TermKey.new
ret, key = tk.waitkey

case key.type
when TermKey::TYPE_UNICODE
  puts "type unicode #{key.code}"
when TermKey::TYPE_FUNCTION
  puts "type function #{key.code}"
when TermKey::TYPE_KEYSYM
  puts "type keysym #{key.code}"
when TermKey::TYPE_MOUSE
  puts "type mouse #{key.code}"
else
  puts key.type
end

if key.modifiers & TermKey::KEYMOD_SHIFT > 0
	puts "shift"
end
if key.modifiers & TermKey::KEYMOD_ALT > 0
	puts "alt"
end
if key.modifiers & TermKey::KEYMOD_CTRL > 0
	puts "ctrl"
end
puts tk.strfkey(key, TermKey::FORMAT_VIM);

