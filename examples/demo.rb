#  TERMKEY_CHECK_VERSION;

mouse = 0
mouse_proto = 0
format = TermKey::FORMAT_VIM

if ARGV.size > 0
  if ARGV[0] == "-m"
    mouse = 1000
  end
end
#  int opt;
#  while((opt = getopt(argc, argv, "m::p:")) != -1) {
#    switch(opt) {
#    case 'm':
#      if(optarg)
#        mouse = atoi(optarg);
#      else
#        mouse = 1000;
#
#      break;
#
#    case 'p':
#      mouse_proto = atoi(optarg);
#      break;
#
#    default:
#      fprintf(stderr, "Usage: %s [-m]\n", argv[0]);
#      return 1;
#    }
#  }

tk = TermKey.new(0, TermKey::FLAG_SPACESYMBOL | TermKey::FLAG_CTRLC)

if tk == nil
  $stderr.puts "Cannot allocate termkey instance\n"
  exit
end

if tk.get_flags & TermKey::FLAG_UTF8
  puts "Termkey in UTF-8 mode"
elsif tk.get_flags & TermKey::FLAG_RAW
  puts "Termkey in RAW mode"
end

if mouse > 0
  printf "\033[?%dhMouse mode active\n", mouse
  if mouse_proto > 0
    printf "\033[?%dh", mouse_proto
  end
end

loop do
  ret, key = tk.waitkey
  if ret == TermKey::RES_EOF
    break
  end

  if ret == TermKey::RES_KEY
    buffer = tk.strfkey(key, format)

    if key.type == TermKey::TYPE_MOUSE
      mouse_ret = tk.interpret_mouse(key)
      printf "%s at line=%d, col=%d\n", buffer, mouse_ret[2], mouse_ret[3]
    else 
      printf "Key %s\n", buffer
    end
    if key.type == TermKey::TYPE_UNICODE and
      (key.modifiers & TermKey::KEYMOD_CTRL) > 0 and
      (key.code.chr == 'C' || key.code.chr == 'c')
      break
    end
    if key.type == TermKey::TYPE_UNICODE and
      key.modifiers == 0 and
      key.code.chr == '?'
    #        // printf("\033[?6n"); // DECDSR 6 == request cursor position
      printf "\033[?1$p" # // DECRQM == request mode, DEC origin mode
      $stdout.flush
    end
  elsif ret == TermKey::RES_ERROR
    printf "termkey_waitkey"
  end
end

if mouse > 0
  printf "\033[?%dlMouse mode deactivated\n", mouse
end

tk.destroy