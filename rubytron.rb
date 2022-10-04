MRuby::Build.new do |conf|
  conf.toolchain

  conf.gem :mgem => 'mruby-regexp-pcre'

  conf.gembox 'default'

  conf.enable_bintest
  conf.enable_test
end
