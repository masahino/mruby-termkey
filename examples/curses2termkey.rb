min = Curses::KEY_MIN
max = Curses::KEY_MAX
tk = TermKey.new
for c in min..max
  kname = Curses::keyname(c)
  if kname != ""
    tmp_name = kname.sub("KEY_", "").tr("()", "").capitalize
    ret = tk.strpkey(tmp_name, 0)
    puts "#{c} -> #{kname} -> #{tmp_name} ->"
    case ret.type
    when TermKey::TYPE_KEYSYM, TermKey::TYPE_FUNCTION
      tkname = tk.strfkey(ret, 0)
      puts "\t#{ret.code} #{tkname}"
    when TermKey::TYPE_UNICODE
      puts "unicode\t#{ret.code}"
    else
      puts "type #{ret.type}"
    end
  end
end