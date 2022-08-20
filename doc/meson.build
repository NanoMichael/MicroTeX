toml_conf = configuration_data()
toml_conf.set('version', meson.project_version())

gidocgen_common_args = [
  '--quiet',
]

docs_dir = get_option('prefix') / get_option('datadir') / 'doc'

if get_option('TARGET_DEVEL')
    source_toml = configure_file(
        input: 'microtex-doc.toml.in',
        output: 'microtex-doc.toml',
        configuration: toml_conf,
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
else
    warning('TARGET_DOCUMENTATION was enabled, but TARGET_DEVEL was not. No documentation was generated')
endif