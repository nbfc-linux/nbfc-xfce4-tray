gtk = dependency('gtk+-3.0')
xfce4panel = dependency('libxfce4panel-2.0')

conf = configuration_data()
conf.set_quoted('DATA_DIR',
  join_paths(
    get_option('prefix'),
    get_option('datadir'),
    meson.project_name()))

configure_file(
  input: 'config.h.in',
  output: 'config.h',
  configuration: conf,
)

shared_module(
    'nbfc',

    [
        'plugin.c',
    ],

    dependencies: [
        gtk,
        xfce4panel,
    ],

    install: true,
    install_dir: join_paths(
        get_option('libdir'),
        'xfce4',
        'panel',
        'plugins',
    ),
)
