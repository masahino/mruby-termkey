MRuby::Build.new do |conf|
  toolchain :gcc
  conf.gembox 'default'
  conf.gem '../mruby-termkey' do |g|
    g.download_libtermkey
  end
end
