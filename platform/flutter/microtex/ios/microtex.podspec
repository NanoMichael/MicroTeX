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
  s.source_files = 'Classes/**/*'
  s.dependency 'Flutter'
  s.platform = :ios, '9.0'

  s.libraries = 'c++'
  s.vendored_libraries = 'libs/libmicrotex.a'

  # Flutter.framework does not contain a i386 slice.
  s.pod_target_xcconfig = { 'DEFINES_MODULE' => 'YES', 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'i386 arm64' }
  s.user_target_xcconfig = { 'EXCLUDED_ARCHS[sdk=iphonesimulator*]' => 'arm64' }
  s.swift_version = '5.0'
end
