Pod::Spec.new do |s|
  s.name = 'ZipArchive.swift'
  s.version = '0.1.0'
  s.summary = 'Zip archiving library written in Swift and C.'
  s.homepage = 'https://github.com/yaslab/ZipArchive.swift'
  s.license = 'MIT'
  s.author = { 'Yasuhiro Hatta' => 'hatta.yasuhiro@gmail.com' }
  s.source = { :git => 'https://github.com/yaslab/ZipArchive.swift.git', :tag => s.version }
  #s.social_media_url = 'https://twitter.com/...'

  s.ios.deployment_target = '8.0'
  s.osx.deployment_target = '10.9'

  s.module_name = 'ZipArchive'
  s.source_files = 'Sources/ZipArchive/*.swift', 'Sources/CoreZipArchive/*.c', 'Sources/CoreZipArchive/include/*.h'
  s.private_header_files = 'Sources/CoreZipArchive/include/*.h'
  s.preserve_paths = 'Sources/CoreZipArchive/include/module.modulemap'

  s.libraries = 'z'
  s.xcconfig = { 'HEADER_SEARCH_PATHS' => '$(SDKROOT)/usr/include', 'SWIFT_INCLUDE_PATHS' => '$(PODS_ROOT)/ZipArchive.swift/Sources/CoreZipArchive/include/' }
end
