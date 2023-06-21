#include "platform_none.cpp"

#include <stdio.h>
#include <unistd.h>

#include <microtex.h>
#include <utils/exceptions.h>

#define FONT_SIZE 18.f

#ifndef __AFL_FUZZ_TESTCASE_LEN
	ssize_t fuzz_len;
	#define __AFL_FUZZ_TESTCASE_LEN fuzz_len
	unsigned char fuzz_buf[1024000];
	#define __AFL_FUZZ_TESTCASE_BUF fuzz_buf
	#define __AFL_FUZZ_INIT() void sync(void);
	#define __AFL_LOOP(x) ((fuzz_len = read(0, fuzz_buf, sizeof(fuzz_buf))) > 0 ? 1 : 0)
	#define __AFL_INIT() sync()
#endif

__AFL_FUZZ_INIT();
int main(void) {
	microtex::Init init = microtex::InitFontSenseAuto{};
	microtex::MicroTeX::init(init);

	microtex::PlatformFactory::registerFactory(
		"none",
		std::make_unique<microtex::PlatformFactory_none>()
	);
	microtex::PlatformFactory::activate("none");

#ifdef __AFL_HAVE_MANUAL_CONTROL
	__AFL_INIT();
#endif
	unsigned char* buf = __AFL_FUZZ_TESTCASE_BUF;
	while (__AFL_LOOP(10000)) {
		int len = __AFL_FUZZ_TESTCASE_LEN;
		if (len < 2) // assume that single characters are always working
			continue;
		try {
			microtex::Render* render = microtex::MicroTeX::parse(
				std::string((char*)buf, len),
				0,
				FONT_SIZE,
				FONT_SIZE / 3.f,
				0xff000000
			);

			microtex::Graphics2D_none g2;
			render->draw(g2, 0, 0);

			delete render;
		} catch (microtex::ex_parse& e) {
			fprintf(stderr, "MicroTeX parsing error: %s\n", e.what());
		} catch (microtex::ex_invalid_state& e) {
			fprintf(stderr, "MicroTeX rendering error: %s\n", e.what());
		}
	}

	microtex::MicroTeX::release();
}
