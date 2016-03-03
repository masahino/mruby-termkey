MRuby::Gem::Specification.new('mruby-termkey') do |spec|
  spec.license = 'MIT'
  spec.authors = 'masahino'
  spec.linker.libraries << 'termkey'

  def spec.download_libtermkey
    require 'open-uri'
    libtermkey_url = "http://www.leonerd.org.uk/code/libtermkey/libtermkey-0.17.tar.gz"
    libtermkey_build_root = "build/libtermkey/#{build.name}"
    libtermkey_dir = "#{libtermkey_build_root}/libtermkey-0.17"
    libtermkey_a = "#{libtermkey_dir}/.libs/libtermkey.a"

    unless File.exists?(libtermkey_a)
      open(libtermkey_url, "r") do |http|
        libtermkey_tar = http.read
        FileUtils.mkdir_p libtermkey_build_root
        IO.popen("tar xfz - -C #{filename libtermkey_build_root}", "w") do |f|
          f.write libtermkey_tar
        end
        sh %Q{(cd #{filename libtermkey_dir} && CC=#{build.cc.command} CFLAGS="#{build.cc.all_flags.gsub('\\','\\\\').gsub('"', '\\"')}" make)}
      end
    end

    self.linker.flags_before_libraries << libtermkey_a
    self.linker.libraries.delete 'termkey'
    [self.cc, self.cxx, self.objc, self.mruby.cc, self.mruby.cxx, self.mruby.objc].each do |cc|
      cc.include_paths << libtermkey_dir
    end
  end
end
