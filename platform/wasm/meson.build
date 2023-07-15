subdir('lib/wrapper')

executable('microtex-wasm', 'main.cpp',
	dependencies: [microtex_dep, microtex_cwrapper_dep],
	link_args: ['-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sWASM=1', '-sEXPORTED_RUNTIME_METHODS=allocateUTF8,UTF8ToString,addFunction',
		'-sEXPORTED_FUNCTIONS=_malloc,_free', '-sIMPORTED_MEMORY', '-sALLOW_MEMORY_GROWTH=1', '-sRESERVED_FUNCTION_POINTERS=4'
	]
)
