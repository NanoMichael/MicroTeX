#include "platform_none.cpp"

#include <stdio.h>
#include <iostream>

#include <microtex.h>
#include <utils/exceptions.h>

#define FONT_SIZE 18.f

int main(void) {
	microtex::Init init = microtex::InitFontSenseAuto{};
	microtex::MicroTeX::init(init);

	microtex::PlatformFactory::registerFactory(
		"none",
		std::make_unique<microtex::PlatformFactory_none>()
	);
	microtex::PlatformFactory::activate("none");

	std::istreambuf_iterator<char> begin(std::cin), end;
	try {
		microtex::Render* render = microtex::MicroTeX::parse(
			std::string (begin, end),
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


	/*char* line = NULL;
	size_t line_len = 0;
	size_t read;
	while ((read = getline(&line, &line_len, stdin)) != -1) {
		try {
			microtex::Render* render = microtex::MicroTeX::parse(
				std::string(line, line_len),
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
	if (line)
		free(line);*/

	microtex::MicroTeX::release();
}
