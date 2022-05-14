#import "MicrotexPlugin.h"
#if __has_include(<microtex/microtex-Swift.h>)
#import <microtex/microtex-Swift.h>
#else
// Support project import fallback if the generated compatibility header
// is not copied when this plugin is created as a library.
// https://forums.swift.org/t/swift-static-libraries-dont-copy-generated-objective-c-header/19816
#import "microtex-Swift.h"
#endif

#import "dummy.h"

@implementation MicrotexPlugin
+ (void)registerWithRegistrar:(NSObject<FlutterPluginRegistrar>*)registrar {
  [SwiftMicrotexPlugin registerWithRegistrar:registrar];
  // dummy function call to keep microtex lib
  microtex_isLittleEndian();
}
@end
