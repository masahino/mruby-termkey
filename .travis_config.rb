MRuby::Build.new do |conf|
  toolchain :gcc
  conf.gembox 'default'
  conf.gem '../mruby-libtermkey' do |g|
    g.download_libtermkey
  end
end
