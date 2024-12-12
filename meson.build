project('inkmlviewer', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3',
                     'cpp_std=c++17'])

gtkmm_dep = dependency('gtkmm-4.0')
xmlpp_dep = dependency('libxml++-5.0')

program_name = 'inkmlviewer'
cpp_sources = [
  'main.cpp',
  'inkmlviewer.cpp',
  'inkmlviewer.h',
  'myxmlparser.h',
  'myxmlparser.cpp',
]

exe = executable(program_name, cpp_sources,
  dependencies: [gtkmm_dep, xmlpp_dep],
  install : true)

test('basic', exe)

# Create desktop shortcut

python3 = import('python').find_installation()
install_cmd = meson.current_source_dir() / 'install-cmd.py'

conf_data = configuration_data()
conf_data.set('bindir', meson.current_build_dir())
desktop_file = configure_file(
  input: 'inkmlviewer.desktop.in',
  output: '@BASENAME@',
  configuration: conf_data,
)

run_target('install-desktop-file',
  command: [
    python3, install_cmd, 'desktop',
    program_name,
    desktop_file,
    meson.current_source_dir() / 'inkmlviewer.png',
  ]
)

