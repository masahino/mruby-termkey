MRuby::Gem::Specification.new('mruby-termkey') do |spec|
  spec.license = 'MIT'
  spec.authors = 'masahino'
  spec.linker.libraries << 'termkey'

  def spec.download_libtermkey
    require 'open-uri'
    libtermkey_url = "http://www.leonerd.org.uk/code/libtermkey/libtermkey-0.18.tar.gz"
    libtermkey_build_root = "#{build_dir}/libtermkey/"
    libtermkey_dir = "#{libtermkey_build_root}/libtermkey-0.18"
    libtermkey_a = "#{libtermkey_dir}/libtermkey.a"

    unless File.exists?(libtermkey_a)
      open(libtermkey_url, "r") do |http|
        libtermkey_tar = http.read
        FileUtils.mkdir_p libtermkey_build_root
        IO.popen("tar xfz - -C #{filename libtermkey_build_root}", "w") do |f|
          f.write libtermkey_tar
        end
        if build.kind_of?(MRuby::CrossBuild) && %w(x86_64-apple-darwin14 i386-apple-darwin14 x86_64-w64-mingw32 i686-w64-mingw32).include?(build.host_target)
          if %w(x86_64-w64-mingw32 i686-w64-mingw32).include?(build.host_target)
#            sh %Q{cd #{libtermkey_build_root} && patch -p0 < #{dir}/libtermkey-0.18.patch}
            sh %Q{cd #{libtermkey_dir} && wget http://foicica.com/hg/textadept/raw-file/ecbc553cbbc7/src/termkey.patch && patch < termkey.patch}
          end
          sh %Q{(cd #{filename libtermkey_dir} && CC=#{build.cc.command} CFLAGS="#{build.cc.all_flags.gsub('\\','\\\\').gsub('"', '\\"')}"  make termkey.o)}
          if %w(x86_64-w64-mingw32 i686-w64-mingw32).include?(build.host_target)
            sh %Q{(cd #{filename libtermkey_dir} && CC=#{build.cc.command} CFLAGS="#{build.cc.all_flags.gsub('\\','\\\\').gsub('"', '\\"')}"  make driver-win32-pdcurses.o)}
            sh %Q{(cd #{filename libtermkey_dir} && #{build.archiver.command} cru libtermkey.a termkey.o driver-win32-pdcurses.o)}
          else
            sh %Q{(cd #{filename libtermkey_dir} && CC=#{build.cc.command} CFLAGS="#{build.cc.all_flags.gsub('\\','\\\\').gsub('"', '\\"')}"  make driver-csi.o)}
            sh %Q{(cd #{filename libtermkey_dir} && CC=#{build.cc.command} CFLAGS="#{build.cc.all_flags.gsub('\\','\\\\').gsub('"', '\\"')}"  make driver-ti.o)}
            sh %Q{(cd #{filename libtermkey_dir} && #{build.archiver.command} cru libtermkey.a termkey.o driver-csi.o driver-ti.o)}
          end
          sh %Q{(cd #{filename libtermkey_dir} && #{build.host_target}-ranlib libtermkey.a)}

        else
          sh %Q{(cd #{filename libtermkey_dir} && CC=#{build.cc.command} CFLAGS="#{build.cc.all_flags.gsub('\\','\\\\').gsub('"', '\\"')}" LDFLAGS="#{build.linker.all_flags.gsub('\\','\\\\').gsub('"', '\\"')}" make)}
          sh %Q{(cd #{filename libtermkey_dir} && cp .libs/libtermkey.a ./libtermkey.a)}
        end
      end
    end

    self.linker.flags_before_libraries << libtermkey_a
    self.linker.libraries.delete 'termkey'
    [self.cc, self.cxx, self.objc, self.mruby.cc, self.mruby.cxx, self.mruby.objc].each do |cc|
      cc.include_paths << libtermkey_dir
    end
  end
end
