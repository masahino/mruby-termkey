MRuby::Gem::Specification.new('mruby-termkey') do |spec|
  spec.license = 'MIT'
  spec.authors = 'masahino'
  spec.linker.libraries << 'termkey'

  def self.run_command(env, command)
    fail "#{command} failed" unless system(env, command)
  end

  def spec.download_libtermkey
    require 'open-uri'
    libtermkey_url = "http://www.leonerd.org.uk/code/libtermkey/libtermkey-0.22.tar.gz"
    libtermkey_build_root = "#{build_dir}/libtermkey/"
    libtermkey_dir = "#{libtermkey_build_root}/libtermkey-0.22"
    libtermkey_a = "#{libtermkey_dir}/libtermkey.a"

    unless File.exists?(libtermkey_a)
      unless File.exists?(libtermkey_dir)
        open(libtermkey_url, "r") do |http|
          libtermkey_tar = http.read
          FileUtils.mkdir_p libtermkey_build_root
          IO.popen("tar xfz - -C #{filename libtermkey_build_root}", "wb") do |f|
            f.write libtermkey_tar
          end
        end
      end
      Dir.chdir(libtermkey_dir) do |t|
        if build.kind_of?(MRuby::CrossBuild) && %w(x86_64-w64-mingw32).include?(build.host_target)
          build.cc.flags << "-DHAVE_UNIBILIUM"
        end
        e = {
          'CC'  => "#{build.cc.command} #{build.cc.flags.join(' ')}",
          'CXX' => "#{build.cxx.command} #{build.cxx.flags.join(' ')}",
          'LD'  => "#{build.linker.command} #{build.linker.flags.join(' ')}",
          'AR'  => build.archiver.command
        }
        if RUBY_PLATFORM.downcase =~ /msys|mingw/ or
          (build.kind_of?(MRuby::CrossBuild) && %w(x86_64-w64-mingw32).include?(build.host_target))
            sh %Q{patch -N -p1 < #{dir}/libtermkey-0.22.patch}
        end
        if build.kind_of?(MRuby::CrossBuild) &&
          %w(x86_64-apple-darwin14 x86_64-apple-darwin19 x86_64-w64-mingw32 arm-linux-gnueabihf).include?(build.host_target)
          run_command e, "make termkey.o"
          run_command e, "make driver-csi.o"
          run_command e, "make driver-ti.o"
#          sh %Q{(cd #{filename libtermkey_dir} && CC=#{build.cc.command} CFLAGS="-DHAVE_UNIBILIUM" make termkey.o)}
#          sh %Q{(cd #{filename libtermkey_dir} && CC=#{build.cc.command} make driver-csi.o)}
#          sh %Q{(cd #{filename libtermkey_dir} && CC=#{build.cc.command} make driver-ti.o)}
          sh %Q{(cd #{filename libtermkey_dir} && #{build.archiver.command} cru libtermkey.a termkey.o driver-csi.o driver-ti.o)}
          sh %Q{(cd #{filename libtermkey_dir} && #{build.host_target}-ranlib libtermkey.a)}
        else
          run_command e, 'make libtermkey.la'
          sh %Q{cp .libs/libtermkey.a ./libtermkey.a}
        end
      end
    end

    self.linker.flags_before_libraries << libtermkey_a
    self.linker.libraries.delete 'termkey'
    [self.cc, self.cxx, self.objc, self.mruby.cc, self.mruby.cxx, self.mruby.objc].each do |cc|
      cc.include_paths << libtermkey_dir
    end
    if RUBY_PLATFORM.downcase =~ /msys|mingw/ or
	(build.kind_of?(MRuby::CrossBuild) && %w(x86_64-w64-mingw32).include?(build.host_target))
      self.linker.libraries << 'unibilium'
    else
      self.linker.libraries << 'ncurses'
    end
  end
end
