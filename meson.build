project(
    'nbfc-xfce4-tray',
    'c',
    version: '1.0.0',
    meson_version: '>=0.60'
)

subdir('src')

install_data(
    'nbfc-xfce4-tray.desktop',
    install_dir: join_paths(
        get_option('datadir'),
        'xfce4',
        'panel',
        'plugins',
    ),
)

install_data(
    'tray.ico',
    install_dir: join_paths(
        get_option('datadir'),
        meson.project_name(),
    ),
)
