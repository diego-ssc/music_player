project('music_player', 'cpp',
        version : '0.1',
        default_options : ['warning_level=3'])

add_global_arguments('-lpthread',
                     '-rdynamic',
                     language : 'cpp') #-g

gtk = dependency('gtkmm-4.0')
threads = dependency('threads')
glibmm = dependency('glibmm-2.68')
gstreamermm = dependency('gstreamer-1.0')
gst_dep = dependency('gstreamer-1.0', version : '>=@0@'.format('1.8.0'))
gmodule_dep = dependency('gmodule-2.0', version : '>=@0@'.format('2.41.1'))
gio_dep = dependency('gio-2.0', version : '>=@0@'.format('2.41.1'))
catch2 = subproject('catch2').get_variable('catch2_dep')

src = [
  'src/Miner.cc',
  'src/Player.cc',
  'src/Music_Player-main.cc'
]

includes = include_directories('src/')

music_player = static_library('music_player', src,
                              dependencies: [ gtk, threads, glibmm,
                                              gstreamermm, gst_dep,
                                              gmodule_dep, gio_dep ],
                              include_directories: [ includes ],
                              install : true)

exe = executable('music_player', src,
                 dependencies: [ gtk, threads, glibmm,
                                 gstreamermm, gst_dep,
                                 gmodule_dep, gio_dep ],
                 install : true)

catch_sources = [ 'test/test_miner.cc' ]
catch_exe = executable ( 'test_miner', catch_sources,
                         dependencies : [ catch2 ],
                         include_directories: [ includes ],
                         link_with: [ music_player ])

test ('Test miner', catch_exe)