MRuby::Gem::Specification.new('mruby-sdl') do |spec|
  spec.license = 'MIT'
  spec.authors = 'Ryan Scott'
  spec.version = '0.1.0'

  spec.cc.include_paths << "/Users/administrator/SDL/include"

  spec.cc.include_paths << "/usr/include/GL"

  spec.linker.library_paths << "/Users/administrator/SDL/lib"
  spec.linker.library_paths << "/System/Library/Frameworks/OpenGL.framework/Libraries"
  spec.linker.library_paths << "/usr/lib"

  spec.linker.libraries << %w(SDL2 GLEW GL)

  if ENV['OS'] == 'Windows_NT'
    spec.linker.libraries << %w(SDLmain)
  end

  spec.requirements << "SDL >= 2.0.0"
end
