MRuby::Gem::Specification.new('mruby-termkey') do |spec|
  spec.license = 'MIT'
  spec.authors = 'mruby-termkey developers'
  spec.linker.libraries << 'termkey'
end
