project(
    'app',
    'c',
    'cpp',
    version: '0.0.0',
    default_options: ['c_std=c99', 'cpp_std=c++20'],
    meson_version: '>=1.3.2',
)

c_args = []
cpp_args = []
c_link_args = []
project_src = []
project_inc = []
link_deps = []

size = find_program('size')
objcopy = find_program('objcopy')

args = [
    '-DPLATFORM_' + get_option('platform'),
    '-DRTOS_' + get_option('rtos').to_upper(),
    '-fno-exceptions',
    '-fomit-frame-pointer',
    '-ggdb3',
    '-Wpedantic',
    '-Wall',
    '-Wextra',
    '-Wundef',
    '-Wdouble-promotion',
    '-Werror',
    '-c',
    '-fno-common',
    '-specs=nano.specs',
    '-Os',
]

message(
    '-DPLATFORM_' + get_option('platform'),
)

c_args += ['-std=c99', '-Wstrict-prototypes'] + args
cpp_args += ['-std=c++20'] + args
c_link_args += [
    '-nostartfiles',
    '--entry=Reset_Handler',
    '-ggdb3',
    '-Wl,-Map=' + meson.project_name() + '.map',
    '-specs=nano.specs',
    '-lc',
    '-lm',
    '-lgcc',
    '-Os',
]

if get_option('save-temps')
    c_args += ['-save-temps']
    cpp_args += ['-save-temps']
endif

startupasm = files('drivers/' + get_option('driver') + '/startup/startup.S')
linkscript = '-T../../drivers/' + get_option('driver') + '/startup/linker_flash.ld'

io_inc = '-I' + (meson.current_build_dir() / 'include')
c_args += io_inc
cpp_args += io_inc

subdir('build')

if get_option('test')
    gtest = subproject('gtest')
    gtest_dep = gtest.get_variable('gtest_dep')
    gmock_dep = gtest.get_variable('gmock_dep')

    utilities = library(
        'utilities',
        [
            'application/source/error.cpp',
            'application/source/utility.cpp',
        ],
        c_args: io_inc,
        cpp_args: ['-DTESTING', '-std=c++20', io_inc],
        include_directories: [project_inc, test_inc],
    )

    foreach test_cpp : test_src
        test_name = test_cpp.split('/')[-1].split('.')[0]
        source_cpp = test_cpp.split('/')[-1].split('_test')[0] + '.cpp'

        test_exec = executable(
            test_name,
            [
                test_cpp,
                'application/source/' + source_cpp,
                'test/unit/source/main.cpp',
            ],
            c_args: io_inc,
            cpp_args: ['-DTESTING', '-std=c++20', io_inc],
            dependencies: [gtest_dep, gmock_dep],
            include_directories: [project_inc, test_inc],
            link_with: utilities,
        )

        test(
            test_name,
            test_exec,
        )
    endforeach
else
    elf = executable(
        meson.project_name(),
        [project_src, startupasm],
        name_suffix: 'elf',
        c_args: c_args,
        cpp_args: cpp_args,
        link_args: [c_link_args, linkscript],
        dependencies: link_deps,
        include_directories: [project_inc],
        native: false,
    )

    custom_target(
        'binary',
        output: meson.project_name() + '.bin',
        build_by_default: true,
        command: [
            objcopy,
            [
                '-O', 'binary',
                '-S', elf.full_path(),
                meson.project_name() + '.bin',
            ],
        ],
        depends: elf,
    )

    run_target('size', command: [size, elf.full_path()], depends: elf)
endif