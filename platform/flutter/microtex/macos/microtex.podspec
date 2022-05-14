#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html.
# Run `pod lib lint microtex.podspec` to validate before publishing.
#
Pod::Spec.new do |s|
  s.name             = 'microtex'
  s.version          = '0.0.1'
  s.summary          = 'A micro, fast, cross-platform, and embeddable LaTeX rendering library.'
  s.description      = <<-DESC
A micro, fast, cross-platform, and embeddable LaTeX rendering library.
                       DESC
  s.homepage         = 'http://example.com'
  s.license          = { :file => '../LICENSE' }
  s.author           = { 'Your Company' => 'email@example.com' }
  s.source           = { :path => '.' }
  s.source_files     = 'Classes/**/*'
  s.dependency 'FlutterMacOS'

  s.platform = :osx, '10.11'

  s.libraries = 'c++'
  s.vendored_libraries = 'libs/libmicrotex-flutter.a', 'libs/libmicrotex.a'

  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES' }
  s.swift_version = '5.0'
end
