doc_conf = configuration_data()
doc_conf.set('version', meson.project_version())
doc_conf.set('cwd', meson.current_source_dir())
doc_conf.set('source_root', meson.source_root())

gidocgen_common_args = [
  '--quiet',
]

docs_dir = get_option('prefix') / get_option('datadir') / 'doc'

if get_option('TARGET_DEVEL')
    source_toml = configure_file(
        input: 'microtex-doc.toml.in',
        output: 'microtex-doc.toml',
        configuration: doc_conf,
        install: true,
        install_dir: docs_dir / 'microtex',
    )

    gidocgen = find_program('gi-docgen', required: get_option('TARGET_DOCUMENTATION'))
    custom_target('microtex-doc',
        input: [source_toml, gtk_gir[0]],
        output: 'microtex',
        command: [
            gidocgen,
            'generate',
            gidocgen_common_args,
            '--config=@INPUT0@',
            '--output-dir=@OUTPUT@',
            '--content-dir=@0@'.format(meson.current_source_dir()),
            '@INPUT1@',
        ],
        build_by_default: true,
        install: true,
        install_dir: docs_dir
    )

    # doxygen
    doxyfile = configure_file(
        input: 'microtex.doxy.in',
        output: 'microtex.doxy',
        configuration: doc_conf,
        install: true,
        install_dir: docs_dir / 'microtex',
    )
    doxygen = find_program('doxygen', required: get_option('TARGET_DOCUMENTATION'))
    custom_target('doxygen',
      input: doxyfile,
      output: 'html',
      command: [doxygen, '@INPUT0@'],
      depend_files: ['mainpage.h', 'cairodoc.h', 'fontdoc.h', 'microtex.docmacros.tex', 'theme/header.html', 'theme/style.css'],
      install: true,
      build_by_default: true,
      install_dir: docs_dir / 'microtex'
    )
else
    warning('TARGET_DOCUMENTATION was enabled, but TARGET_DEVEL was not. No documentation was generated')
endif
